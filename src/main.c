#include <stdio.h>

#include "hal_imu.h"
#include "hal_motors.h"
#include "hal_receiver.h"
#include "hal_time.h"
#include "time_sim.h"
#include "control/mixer.h"
#include "safety/arming.h"

#define LOOP_US 1000u
#define SIM_STEPS 1000u

int main(void)
{
    printf("=============================\n");
    printf("       AIR65 SIM BOOT OK\n");
    printf("=============================\n");

    bool imu_ok = imu_init();
    bool rx_ok = receiver_init();
    motors_init();
    safety_init();

    printf("IMU:      %s\n", imu_ok ? "SIMULATED" : "FAIL");
    printf("Receiver: %s\n", rx_ok ? "SIMULATED" : "FAIL");
    printf("Motors:   SIMULATED\n\n");

    rc_input_t rc = {0};
    imu_sample_t imu = {0};
    motor_cmd_t cmd = {0};
    arm_state_t state = ARM_STATE_DISARMED;

    for (uint32_t step = 0; step < SIM_STEPS; step++) {
        receiver_read(&rc);
        imu_ok = imu_read(&imu);
        state = safety_update(&rc, imu_ok, time_us());

        if (state == ARM_STATE_ARMED) {
            mixer_update(rc.throttle, 0.0f, 0.0f, 0.0f, &cmd);
            motors_write(&cmd);
        } else {
            motors_stop();
        }

        time_sim_advance(LOOP_US);
    }

    printf("Ran %u loops (%.1f s simulated)\n", SIM_STEPS, SIM_STEPS * LOOP_US / 1e6);
    printf("Final state: %s\n",
           state == ARM_STATE_ARMED ? "ARMED" :
           state == ARM_STATE_FAILSAFE ? "FAILSAFE" : "DISARMED");
    return 0;
}
