#include "hal_imu.h"

bool imu_init(void)
{
    return true;
}

bool imu_read(imu_sample_t *out)
{
    out->gyro_dps = (vec3_t){0.0f, 0.0f, 0.0f};
    out->accel_g = (vec3_t){0.0f, 0.0f, 1.0f};
    out->valid = true;
    return true;
}
