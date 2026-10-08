#include "hal_receiver.h"
#include "hal_time.h"

bool receiver_init(void)
{
    return true;
}

void receiver_read(rc_input_t *out)
{
    out->roll = 0.0f;
    out->pitch = 0.0f;
    out->yaw = 0.0f;
    out->throttle = 0.0f;
    out->arm_switch = false;
    out->link_ok = true;
    out->last_packet_us = time_us();
}
