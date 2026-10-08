#include "hal_time.h"
#include "time_sim.h"

static uint32_t sim_now_us = 0;

uint32_t time_us(void)
{
    return sim_now_us;
}

void time_sim_advance(uint32_t us)
{
    sim_now_us += us;
}

void time_sim_reset(void)
{
    sim_now_us = 0;
}
