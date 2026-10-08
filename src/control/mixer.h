#ifndef MIXER_H
#define MIXER_H

#include "types.h"

void mixer_update(float throttle, float roll, float pitch, float yaw, motor_cmd_t *out);

#endif
