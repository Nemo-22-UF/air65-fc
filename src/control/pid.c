#include "pid.h"
#include <math.h>

#define PID_MAX_RATE_DPS 800.0f

static float pid_clamp(float value, float limit)
{
    if (value > limit) return limit;
    if (value < -limit) return -limit;
    return value;
}

void pid_init(pid_ctrl_t *p, float kp, float ki, float kd, float i_limit, float out_limit)
{
    if (!p) return;

    p->kp = isfinite(kp) && kp >= 0.0f ? kp : 0.0f;
    p->ki = isfinite(ki) && ki >= 0.0f ? ki : 0.0f;
    p->kd = isfinite(kd) && kd >= 0.0f ? kd : 0.0f;
    p->i_limit = isfinite(i_limit) && i_limit >= 0.0f ? i_limit : 0.0f;
    p->out_limit = isfinite(out_limit) && out_limit >= 0.0f ? out_limit : 0.0f;
    p->d_lpf_alpha = 1.0f;
    pid_reset(p);
}

float pid_update(pid_ctrl_t *p, float setpoint, float measurement, float dt)
{
    if (!p) return 0.0f;
    if (!isfinite(setpoint) || !isfinite(measurement) || !isfinite(dt) || dt <= 0.0f) {
        pid_reset(p);
        return 0.0f;
    }

    setpoint = pid_clamp(setpoint, PID_MAX_RATE_DPS);
    const float error = setpoint - measurement;
    const float proportional = p->kp * error;

    // Derivative of measurement avoids a spike when the pilot moves a stick.
    float derivative = 0.0f;
    if (p->has_prev_meas) {
        const float alpha = isfinite(p->d_lpf_alpha) && p->d_lpf_alpha >= 0.0f
                         && p->d_lpf_alpha <= 1.0f ? p->d_lpf_alpha : 1.0f;
        const float rate_change = (measurement - p->prev_meas) / dt;
        p->d_filt += alpha * (rate_change - p->d_filt);
        derivative = -p->kd * p->d_filt;
    }
    p->prev_meas = measurement;
    p->has_prev_meas = true;

    // Integral is stored in output units; stop accumulating into saturation.
    const float delta_i = p->ki * error * dt;
    const float candidate_i = pid_clamp(p->integ + delta_i, p->i_limit);
    const float candidate_output = proportional + candidate_i + derivative;

    if (!isfinite(proportional) || !isfinite(derivative) || !isfinite(candidate_output)) {
        pid_reset(p);
        return 0.0f;
    }
    if (!((candidate_output > p->out_limit && delta_i > 0.0f) ||
          (candidate_output < -p->out_limit && delta_i < 0.0f))) {
        p->integ = candidate_i;
    }

    return pid_clamp(proportional + p->integ + derivative, p->out_limit);
}

void pid_reset(pid_ctrl_t *p)
{
    if (!p) return;
    p->integ = 0.0f;
    p->prev_meas = 0.0f;
    p->d_filt = 0.0f;
    p->has_prev_meas = false;
}
