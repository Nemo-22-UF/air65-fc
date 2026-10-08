#ifndef ARMING_H
#define ARMING_H

#include "types.h"

void safety_init(void);
arm_state_t safety_update(const rc_input_t *rc, bool imu_ok, uint32_t now_us);

#endif
