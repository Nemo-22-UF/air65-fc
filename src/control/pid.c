#include "pid.h"

void pid_init(pid_ctrl_t *p, float kp, float ki, float kd, float i_limit, float out_limit)
{
    p->kp = kp;
    p->ki = ki;
    p->kd = kd;
    p->i_limit = i_limit;
    p->out_limit = out_limit;
    p->d_lpf_alpha = 1.0f;
    pid_reset(p);
}

float pid_update(pid_ctrl_t *p, float setpoint, float measurement, float dt)
{
    (void)p;
    (void)setpoint;
    (void)measurement;
    (void)dt;
    return 0.0f;
}

void pid_reset(pid_ctrl_t *p)
{
    p->integ = 0.0f;
    p->prev_meas = 0.0f;
    p->d_filt = 0.0f;
}
