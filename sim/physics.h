#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"

void physics_init(void);
void physics_step(const motor_cmd_t *cmd, float dt);
vec3_t physics_true_rate_dps(void);

#endif
