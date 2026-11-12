#include "test.h"
#include "safety/arming.h"

#include <stddef.h>

/* Healthy link, sticks centered, throttle at zero, switch off, fresh packet. */
static rc_input_t rc_ok(uint32_t now_us)
{
    rc_input_t rc = {0};
    rc.throttle = 0.0f;
    rc.arm_switch = false;
    rc.link_ok = true;
    rc.last_packet_us = now_us;
    return rc;
}

/* Run one update with a fresh packet and the given switch/throttle. */
static arm_state_t step(uint32_t now_us, bool sw, float throttle, bool imu_ok)
{
    rc_input_t rc = rc_ok(now_us);
    rc.arm_switch = sw;
    rc.throttle = throttle;
    return safety_update(&rc, imu_ok, now_us);
}

/* Init, see switch off, then flip it on. Leaves the module ARMED at t = 2000 us. */
static void arm_normally(void)
{
    safety_init();
    step(1000, false, 0.0f, true);
    step(2000, true, 0.0f, true);
}

static void boots_disarmed(void)
{
    safety_init();
    CHECK(step(1000, false, 0.0f, true) == ARM_STATE_DISARMED);
}

static void arms_on_switch_flip_with_low_throttle(void)
{
    safety_init();
    CHECK(step(1000, false, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(2000, true, 0.0f, true) == ARM_STATE_ARMED);
    CHECK(step(3000, true, 0.5f, true) == ARM_STATE_ARMED);
}

static void switch_on_at_boot_does_not_arm(void)
{
    safety_init();
    CHECK(step(1000, true, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(2000, true, 0.0f, true) == ARM_STATE_DISARMED);
    /* Cycling the switch arms. */
    CHECK(step(3000, false, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(4000, true, 0.0f, true) == ARM_STATE_ARMED);
}

static void high_throttle_blocks_arming(void)
{
    safety_init();
    step(1000, false, 0.0f, true);
    CHECK(step(2000, true, 0.5f, true) == ARM_STATE_DISARMED);
    /* Dropping throttle with the switch still on is not enough. */
    CHECK(step(3000, true, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(4000, false, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(5000, true, 0.0f, true) == ARM_STATE_ARMED);
}

static void throttle_threshold_edge(void)
{
    safety_init();
    step(1000, false, 0.0f, true);
    CHECK(step(2000, true, 0.06f, true) == ARM_STATE_DISARMED);
    step(3000, false, 0.0f, true);
    CHECK(step(4000, true, 0.05f, true) == ARM_STATE_ARMED);
}

static void bad_imu_blocks_arming(void)
{
    safety_init();
    step(1000, false, 0.0f, true);
    CHECK(step(2000, true, 0.0f, false) == ARM_STATE_DISARMED);
}

static void bad_link_blocks_arming(void)
{
    rc_input_t rc;

    /* link_ok false */
    safety_init();
    step(1000, false, 0.0f, true);
    rc = rc_ok(2000);
    rc.arm_switch = true;
    rc.link_ok = false;
    CHECK(safety_update(&rc, true, 2000) == ARM_STATE_DISARMED);

    /* Stale packet */
    safety_init();
    step(1000, false, 0.0f, true);
    rc = rc_ok(1000);
    rc.arm_switch = true;
    CHECK(safety_update(&rc, true, 1000 + 200000) == ARM_STATE_DISARMED);
}

static void switch_off_disarms(void)
{
    arm_normally();
    CHECK(step(3000, false, 0.5f, true) == ARM_STATE_DISARMED);
}

static void link_timeout_triggers_failsafe(void)
{
    arm_normally();
    rc_input_t rc = rc_ok(2000);
    rc.arm_switch = true;
    rc.throttle = 0.5f;
    /* 100 ms since last packet: still within the timeout. */
    CHECK(safety_update(&rc, true, 2000 + 100000) == ARM_STATE_ARMED);
    /* Just past it: failsafe. */
    CHECK(safety_update(&rc, true, 2000 + 100001) == ARM_STATE_FAILSAFE);
}

static void link_ok_false_triggers_failsafe_immediately(void)
{
    arm_normally();
    rc_input_t rc = rc_ok(3000);
    rc.arm_switch = true;
    rc.link_ok = false;
    CHECK(safety_update(&rc, true, 3000) == ARM_STATE_FAILSAFE);
}

static void failsafe_recovery_needs_switch_off(void)
{
    arm_normally();
    rc_input_t rc = rc_ok(3000);
    rc.arm_switch = true;
    rc.link_ok = false;
    CHECK(safety_update(&rc, true, 3000) == ARM_STATE_FAILSAFE);

    /* Link back, switch still on: stays in failsafe, never jumps to armed. */
    CHECK(step(4000, true, 0.0f, true) == ARM_STATE_FAILSAFE);
    CHECK(step(5000, true, 0.0f, true) == ARM_STATE_FAILSAFE);

    /* Switch off: disarmed. Then a normal arm works again. */
    CHECK(step(6000, false, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(7000, true, 0.0f, true) == ARM_STATE_ARMED);
}

static void failsafe_recovery_needs_imu(void)
{
    arm_normally();
    for (uint32_t t = 3000; t <= 3000 + 25000; t += 1000) {
        step(t, true, 0.5f, false);
    }
    CHECK(step(29000, false, 0.0f, false) == ARM_STATE_FAILSAFE);
    CHECK(step(30000, false, 0.0f, true) == ARM_STATE_DISARMED);
}

static void short_imu_glitch_is_ignored(void)
{
    arm_normally();
    /* 10 ms of bad IMU, under the 20 ms limit. */
    for (uint32_t t = 3000; t < 13000; t += 1000) {
        CHECK(step(t, true, 0.5f, false) == ARM_STATE_ARMED);
    }
    CHECK(step(13000, true, 0.5f, true) == ARM_STATE_ARMED);
    /* A new glitch starts its own timer. */
    CHECK(step(14000, true, 0.5f, false) == ARM_STATE_ARMED);
}

static void long_imu_failure_triggers_failsafe(void)
{
    arm_normally();
    CHECK(step(3000, true, 0.5f, false) == ARM_STATE_ARMED);
    CHECK(step(3000 + 19000, true, 0.5f, false) == ARM_STATE_ARMED);
    CHECK(step(3000 + 20000, true, 0.5f, false) == ARM_STATE_FAILSAFE);
}

static void switch_seen_during_link_loss_is_ignored(void)
{
    safety_init();
    step(1000, false, 0.0f, true);
    /* Link drops, then returns with the switch already on: must not arm. */
    rc_input_t rc = rc_ok(2000);
    rc.link_ok = false;
    safety_update(&rc, true, 2000);
    CHECK(step(3000, true, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(4000, false, 0.0f, true) == ARM_STATE_DISARMED);
    CHECK(step(5000, true, 0.0f, true) == ARM_STATE_ARMED);
}

static void null_rc_and_nan_throttle_are_safe(void)
{
    safety_init();
    CHECK(safety_update(NULL, true, 1000) == ARM_STATE_DISARMED);

    step(2000, false, 0.0f, true);
    CHECK(step(3000, true, NAN, true) == ARM_STATE_DISARMED);

    arm_normally();
    CHECK(safety_update(NULL, true, 3000) == ARM_STATE_FAILSAFE);
}

static void timer_wraparound(void)
{
    const uint32_t near_wrap = 0xFFFFFFFFu - 500u;
    safety_init();
    step(near_wrap - 1000u, false, 0.0f, true);
    CHECK(step(near_wrap, true, 0.0f, true) == ARM_STATE_ARMED);

    /* Packet sent just before the wrap, checked just after: 1000 us old. */
    rc_input_t rc = rc_ok(near_wrap);
    rc.arm_switch = true;
    rc.throttle = 0.5f;
    CHECK(safety_update(&rc, true, 499u) == ARM_STATE_ARMED);
    /* Well past the timeout after the wrap. */
    CHECK(safety_update(&rc, true, 200000u) == ARM_STATE_FAILSAFE);
}

void run_safety_tests(void)
{
    RUN(boots_disarmed);
    RUN(arms_on_switch_flip_with_low_throttle);
    RUN(switch_on_at_boot_does_not_arm);
    RUN(high_throttle_blocks_arming);
    RUN(throttle_threshold_edge);
    RUN(bad_imu_blocks_arming);
    RUN(bad_link_blocks_arming);
    RUN(switch_off_disarms);
    RUN(link_timeout_triggers_failsafe);
    RUN(link_ok_false_triggers_failsafe_immediately);
    RUN(failsafe_recovery_needs_switch_off);
    RUN(failsafe_recovery_needs_imu);
    RUN(short_imu_glitch_is_ignored);
    RUN(long_imu_failure_triggers_failsafe);
    RUN(switch_seen_during_link_loss_is_ignored);
    RUN(null_rc_and_nan_throttle_are_safe);
    RUN(timer_wraparound);
}
