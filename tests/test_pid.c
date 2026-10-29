#include "test.h"
#include "control/pid.h"

static void p_term_and_output_limits(void)
{
    pid_ctrl_t p;
    pid_init(&p, 0.01f, 0.0f, 0.0f, 0.0f, 0.5f);
    CHECK_NEAR(pid_update(&p, 20.0f, 10.0f, 0.01f), 0.1f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 0.0f, 100.0f, 0.01f), -0.5f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 900.0f, 0.0f, 0.01f), 0.5f, 1e-6f);
}

static void integral_accumulates_and_is_limited(void)
{
    pid_ctrl_t p;
    pid_init(&p, 0.0f, 0.1f, 0.0f, 0.25f, 1.0f);
    CHECK_NEAR(pid_update(&p, 1.0f, 0.0f, 1.0f), 0.1f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 1.0f, 0.0f, 1.0f), 0.2f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 1.0f, 0.0f, 1.0f), 0.25f, 1e-6f);
}

static void derivative_uses_measurement_and_filter(void)
{
    pid_ctrl_t p;
    pid_init(&p, 0.0f, 0.0f, 0.1f, 0.0f, 2.0f);
    p.d_lpf_alpha = 0.5f;
    CHECK_NEAR(pid_update(&p, 50.0f, 10.0f, 1.0f), 0.0f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 50.0f, 20.0f, 1.0f), -0.5f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 100.0f, 20.0f, 1.0f), -0.25f, 1e-6f);
}

static void saturation_stops_integrator_windup(void)
{
    pid_ctrl_t p;
    pid_init(&p, 1.0f, 1.0f, 0.0f, 10.0f, 0.5f);
    for (int i = 0; i < 10; i++) {
        CHECK_NEAR(pid_update(&p, 2.0f, 0.0f, 1.0f), 0.5f, 1e-6f);
    }
    CHECK_NEAR(p.integ, 0.0f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 0.0f, 0.0f, 1.0f), 0.0f, 1e-6f);
}

static void invalid_inputs_and_reset_are_safe(void)
{
    pid_ctrl_t p;
    pid_init(&p, 0.0f, 0.1f, 0.0f, 1.0f, 1.0f);
    pid_update(&p, 1.0f, 0.0f, 1.0f);
    CHECK_NEAR(pid_update(&p, 1.0f, 0.0f, 0.0f), 0.0f, 1e-6f);
    CHECK_NEAR(p.integ, 0.0f, 1e-6f);
    CHECK(!p.has_prev_meas);
    CHECK_NEAR(pid_update(&p, NAN, 0.0f, 1.0f), 0.0f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 1.0f, INFINITY, 1.0f), 0.0f, 1e-6f);
    CHECK_NEAR(pid_update(&p, 1.0f, 0.0f, -1.0f), 0.0f, 1e-6f);
    CHECK_NEAR(pid_update(NULL, 0.0f, 0.0f, 1.0f), 0.0f, 1e-6f);
}

static void axes_have_independent_state(void)
{
    pid_ctrl_t roll, pitch, yaw;
    pid_init(&roll, 0.0f, 0.1f, 0.0f, 1.0f, 1.0f);
    pid_init(&pitch, 0.0f, 0.1f, 0.0f, 1.0f, 1.0f);
    pid_init(&yaw, 0.0f, 0.1f, 0.0f, 1.0f, 1.0f);
    CHECK_NEAR(pid_update(&roll, 2.0f, 0.0f, 1.0f), 0.2f, 1e-6f);
    CHECK_NEAR(pid_update(&pitch, -1.0f, 0.0f, 1.0f), -0.1f, 1e-6f);
    CHECK_NEAR(pid_update(&yaw, 0.0f, 0.0f, 1.0f), 0.0f, 1e-6f);
    pid_reset(&roll);
    CHECK_NEAR(roll.integ, 0.0f, 1e-6f);
    CHECK_NEAR(pitch.integ, -0.1f, 1e-6f);
}

void run_pid_tests(void)
{
    RUN(p_term_and_output_limits);
    RUN(integral_accumulates_and_is_limited);
    RUN(derivative_uses_measurement_and_filter);
    RUN(saturation_stops_integrator_windup);
    RUN(invalid_inputs_and_reset_are_safe);
    RUN(axes_have_independent_state);
}

#ifdef PID_STANDALONE
int test_failures = 0;
int test_checks = 0;

int main(void)
{
    run_pid_tests();
    printf("\n%d checks, %d failed\n", test_checks, test_failures);
    return test_failures == 0 ? 0 : 1;
}
#endif
