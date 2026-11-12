#include "arming.h"

#include <math.h>
#include <stddef.h>

/*
 * Arming and failsafe state machine.
 *
 *   DISARMED --(arm switch off->on, throttle low, link OK, IMU OK)--> ARMED
 *   ARMED    --(arm switch off)--------------------------------------> DISARMED
 *   ARMED    --(link lost or IMU failed)------------------------------> FAILSAFE
 *   FAILSAFE --(link OK, IMU OK, arm switch off)----------------------> DISARMED
 *
 * Main only spins motors in ARMED, so DISARMED and FAILSAFE both mean motors off.
 * FAILSAFE never goes straight back to ARMED: the pilot has to flip the switch
 * off and arm again like normal.
 */

/* Throttle must be at or below this to arm (0..1 scale). */
#define SAFETY_ARM_THROTTLE_MAX 0.05f

/* Link counts as lost if no packet has arrived for this long. */
#define SAFETY_RX_TIMEOUT_US 100000u   /* 100 ms */

/* IMU counts as failed after reporting bad for this long. Short glitches are ignored. */
#define SAFETY_IMU_TIMEOUT_US 20000u   /* 20 ms */

typedef struct {
    arm_state_t state;
    bool prev_switch;        /* arm switch as last seen on a healthy link */
    bool imu_bad;            /* IMU is currently reporting bad */
    uint32_t imu_bad_since_us;
} safety_t;

static safety_t s;

void safety_init(void)
{
    s.state = ARM_STATE_DISARMED;
    /* Start as if the switch was on, so a switch left on at power-up
     * cannot arm. The pilot must flip it off first. */
    s.prev_switch = true;
    s.imu_bad = false;
    s.imu_bad_since_us = 0;
}

/* Unsigned subtraction handles the 32-bit microsecond timer wrapping (~71 min). */
static bool safety_link_ok(const rc_input_t *rc, uint32_t now_us)
{
    if (rc == NULL || !rc->link_ok) {
        return false;
    }
    return (uint32_t)(now_us - rc->last_packet_us) <= SAFETY_RX_TIMEOUT_US;
}

static bool safety_imu_failed(bool imu_ok, uint32_t now_us)
{
    if (imu_ok) {
        s.imu_bad = false;
        return false;
    }
    if (!s.imu_bad) {
        s.imu_bad = true;
        s.imu_bad_since_us = now_us;
    }
    return (uint32_t)(now_us - s.imu_bad_since_us) >= SAFETY_IMU_TIMEOUT_US;
}

static bool safety_throttle_low(float throttle)
{
    /* NaN fails this check, so a bad throttle value can never arm. */
    return isfinite(throttle) && throttle <= SAFETY_ARM_THROTTLE_MAX;
}

arm_state_t safety_update(const rc_input_t *rc, bool imu_ok, uint32_t now_us)
{
    const bool link_ok = safety_link_ok(rc, now_us);
    const bool imu_failed = safety_imu_failed(imu_ok, now_us);

    /* Only trust the arm switch on a healthy link. While the link is down,
     * act as if the switch is on, so it must be seen off again before arming. */
    const bool sw = link_ok ? rc->arm_switch : true;
    const bool switch_rose = sw && !s.prev_switch;
    s.prev_switch = sw;

    switch (s.state) {
    case ARM_STATE_DISARMED:
        if (switch_rose && link_ok && imu_ok && safety_throttle_low(rc->throttle)) {
            s.state = ARM_STATE_ARMED;
        }
        break;

    case ARM_STATE_ARMED:
        if (!link_ok || imu_failed) {
            s.state = ARM_STATE_FAILSAFE;
        } else if (!sw) {
            s.state = ARM_STATE_DISARMED;
        }
        break;

    case ARM_STATE_FAILSAFE:
    default:
        if (link_ok && imu_ok && !sw) {
            s.state = ARM_STATE_DISARMED;
        } else {
            s.state = ARM_STATE_FAILSAFE;
        }
        break;
    }

    return s.state;
}
