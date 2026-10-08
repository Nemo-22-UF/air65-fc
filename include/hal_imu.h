#ifndef HAL_IMU_H
#define HAL_IMU_H

#include "types.h"

bool imu_init(void);
bool imu_read(imu_sample_t *out);

#endif
