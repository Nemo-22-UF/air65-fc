#include "arming.h"

void safety_init(void)
{
}

arm_state_t safety_update(const rc_input_t *rc, bool imu_ok, uint32_t now_us)
{
    (void)rc;
    (void)imu_ok;
    (void)now_us;
    return ARM_STATE_DISARMED;
}
