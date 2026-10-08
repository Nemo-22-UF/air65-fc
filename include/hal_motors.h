#ifndef HAL_MOTORS_H
#define HAL_MOTORS_H

#include "types.h"

void motors_init(void);
void motors_write(const motor_cmd_t *cmd);
void motors_stop(void);

#endif
