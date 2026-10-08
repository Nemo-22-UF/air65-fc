#include "test.h"
#include "control/mixer.h"

static void throttle_only_gives_equal_motors(void)
{
    motor_cmd_t out;
    mixer_update(0.5f, 0.0f, 0.0f, 0.0f, &out);
    for (int i = 0; i < NUM_MOTORS; i++) {
        CHECK_NEAR(out.m[i], 0.5f, 1e-6f);
    }
}

static void zero_throttle_gives_zero_motors(void)
{
    motor_cmd_t out;
    mixer_update(0.0f, 0.0f, 0.0f, 0.0f, &out);
    for (int i = 0; i < NUM_MOTORS; i++) {
        CHECK_NEAR(out.m[i], 0.0f, 1e-6f);
    }
}

void run_mixer_tests(void)
{
    RUN(throttle_only_gives_equal_motors);
    RUN(zero_throttle_gives_zero_motors);
}
