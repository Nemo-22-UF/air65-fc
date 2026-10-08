#include "hal_motors.h"

static motor_cmd_t last_cmd;

void motors_init(void)
{
    motors_stop();
}

void motors_write(const motor_cmd_t *cmd)
{
    last_cmd = *cmd;
}

void motors_stop(void)
{
    for (int i = 0; i < NUM_MOTORS; i++) {
        last_cmd.m[i] = 0.0f;
    }
}
