#include "mixer.h"

void mixer_update(float throttle, float roll, float pitch, float yaw, motor_cmd_t *out)
{
    (void)throttle;
    (void)roll;
    (void)pitch;
    (void)yaw;

    for (int i = 0; i < NUM_MOTORS; i++) {
        out->m[i] = 0.0f;
    }
}
