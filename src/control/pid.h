#ifndef PID_H
#define PID_H

#include <stdbool.h>

typedef struct {
    float kp, ki, kd;
    float i_limit;
    float out_limit;
    float integ;
    float prev_meas;
    float d_filt;
    float d_lpf_alpha;
    bool has_prev_meas;
} pid_ctrl_t;

void pid_init(pid_ctrl_t *p, float kp, float ki, float kd, float i_limit, float out_limit);
float pid_update(pid_ctrl_t *p, float setpoint, float measurement, float dt);
void pid_reset(pid_ctrl_t *p);

#endif
