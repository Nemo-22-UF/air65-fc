#ifndef HAL_RECEIVER_H
#define HAL_RECEIVER_H

#include "types.h"

bool receiver_init(void);
void receiver_read(rc_input_t *out);

#endif
