#include "physics.h"

static vec3_t rate_dps;

void physics_init(void)
{
    rate_dps = (vec3_t){0.0f, 0.0f, 0.0f};
}

void physics_step(const motor_cmd_t *cmd, float dt)
{
    (void)cmd;
    (void)dt;
}

vec3_t physics_true_rate_dps(void)
{
    return rate_dps;
}
