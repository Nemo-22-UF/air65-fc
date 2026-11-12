#include "test.h"
#include "control/mixer.h"

#include <stddef.h>

/* Motor indices (out.m[i]) per CONVENTIONS.md */
#define M1_RR 0   /* rear right,  CW  */
#define M2_FR 1   /* front right, CCW */
#define M3_RL 2   /* rear left,   CCW */
#define M4_FL 3   /* front left,  CW  */

#define TOL 1e-6f

static void check_motors(const motor_cmd_t *out, float m1, float m2, float m3, float m4)
{
    CHECK_NEAR(out->m[M1_RR], m1, TOL);
    CHECK_NEAR(out->m[M2_FR], m2, TOL);
    CHECK_NEAR(out->m[M3_RL], m3, TOL);
    CHECK_NEAR(out->m[M4_FL], m4, TOL);
}

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

/* +roll = right side down -> left motors (M3, M4) speed up */
static void positive_roll_speeds_up_left_motors(void)
{
    motor_cmd_t out;
    mixer_update(0.5f, 0.2f, 0.0f, 0.0f, &out);
    check_motors(&out, 0.3f, 0.3f, 0.7f, 0.7f);
}

static void negative_roll_speeds_up_right_motors(void)
{
    motor_cmd_t out;
    mixer_update(0.5f, -0.2f, 0.0f, 0.0f, &out);
    check_motors(&out, 0.7f, 0.7f, 0.3f, 0.3f);
}

/* +pitch = nose up -> front motors (M2, M4) speed up */
static void positive_pitch_speeds_up_front_motors(void)
{
    motor_cmd_t out;
    mixer_update(0.5f, 0.0f, 0.2f, 0.0f, &out);
    check_motors(&out, 0.3f, 0.7f, 0.3f, 0.7f);
}

/* +yaw = nose right -> counter-clockwise motors (M2, M3) speed up */
static void positive_yaw_speeds_up_ccw_motors(void)
{
    motor_cmd_t out;
    mixer_update(0.5f, 0.0f, 0.0f, 0.2f, &out);
    check_motors(&out, 0.3f, 0.7f, 0.7f, 0.3f);
}

/* Corrections cancel out on average: total thrust stays at 4 x throttle
 * as long as nothing is clipped. */
static void corrections_keep_total_thrust(void)
{
    motor_cmd_t out;
    mixer_update(0.5f, 0.1f, -0.15f, 0.05f, &out);
    float sum = 0.0f;
    for (int i = 0; i < NUM_MOTORS; i++) {
        sum += out.m[i];
    }
    CHECK_NEAR(sum, 2.0f, 1e-5f);
}

/* High throttle + roll: left side would hit 1.2, so everything shifts
 * down 0.2 and the 0.6 left/right difference is kept. */
static void airmode_shift_keeps_steering(void)
{
    motor_cmd_t out;
    mixer_update(0.9f, 0.3f, 0.0f, 0.0f, &out);
    check_motors(&out, 0.4f, 0.4f, 1.0f, 1.0f);
    CHECK_NEAR(out.m[M4_FL] - out.m[M2_FR], 0.6f, 1e-5f);
}

static void full_throttle_with_pitch_still_steers(void)
{
    motor_cmd_t out;
    mixer_update(1.0f, 0.0f, 0.5f, 0.0f, &out);
    /* Front 1.5 -> 1.0, rear 0.5 -> 0.0 */
    check_motors(&out, 0.0f, 1.0f, 0.0f, 1.0f);
}

static void outputs_always_in_range(void)
{
    const float throttles[] = {0.0f, 0.1f, 0.5f, 0.9f, 1.0f};
    const float sticks[] = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
    motor_cmd_t out;

    for (int t = 0; t < 5; t++) {
        for (int r = 0; r < 5; r++) {
            for (int p = 0; p < 5; p++) {
                for (int y = 0; y < 5; y++) {
                    mixer_update(throttles[t], sticks[r], sticks[p], sticks[y], &out);
                    for (int i = 0; i < NUM_MOTORS; i++) {
                        CHECK(out.m[i] >= 0.0f && out.m[i] <= 1.0f);
                    }
                }
            }
        }
    }
}

static void out_of_range_inputs_are_clamped(void)
{
    motor_cmd_t clamped, expected;

    mixer_update(2.0f, 0.0f, 0.0f, 0.0f, &clamped);
    check_motors(&clamped, 1.0f, 1.0f, 1.0f, 1.0f);

    mixer_update(-1.0f, 0.0f, 0.0f, 0.0f, &clamped);
    check_motors(&clamped, 0.0f, 0.0f, 0.0f, 0.0f);

    /* Roll of 5 behaves exactly like roll of 1. */
    mixer_update(0.5f, 5.0f, 0.0f, 0.0f, &clamped);
    mixer_update(0.5f, 1.0f, 0.0f, 0.0f, &expected);
    for (int i = 0; i < NUM_MOTORS; i++) {
        CHECK_NEAR(clamped.m[i], expected.m[i], TOL);
    }
}

static void nan_and_inf_inputs_are_safe(void)
{
    motor_cmd_t out;

    /* Bad throttle -> treated as 0 */
    mixer_update(NAN, 0.0f, 0.0f, 0.0f, &out);
    check_motors(&out, 0.0f, 0.0f, 0.0f, 0.0f);

    /* Bad sticks -> treated as 0, throttle still applied */
    mixer_update(0.5f, NAN, INFINITY, -INFINITY, &out);
    check_motors(&out, 0.5f, 0.5f, 0.5f, 0.5f);
}

static void null_output_does_not_crash(void)
{
    mixer_update(0.5f, 0.1f, 0.1f, 0.1f, NULL);
    CHECK(1);
}

void run_mixer_tests(void)
{
    RUN(throttle_only_gives_equal_motors);
    RUN(zero_throttle_gives_zero_motors);
    RUN(positive_roll_speeds_up_left_motors);
    RUN(negative_roll_speeds_up_right_motors);
    RUN(positive_pitch_speeds_up_front_motors);
    RUN(positive_yaw_speeds_up_ccw_motors);
    RUN(corrections_keep_total_thrust);
    RUN(airmode_shift_keeps_steering);
    RUN(full_throttle_with_pitch_still_steers);
    RUN(outputs_always_in_range);
    RUN(out_of_range_inputs_are_clamped);
    RUN(nan_and_inf_inputs_are_safe);
    RUN(null_output_does_not_crash);
}
