#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    vec3_t gyro_dps;
    vec3_t accel_g;
    bool valid;
} imu_sample_t;

typedef struct {
    float roll, pitch, yaw;
    float throttle;
    bool arm_switch;
    bool link_ok;
    uint32_t last_packet_us;
} rc_input_t;

#define NUM_MOTORS 4

typedef struct {
    float m[NUM_MOTORS];
} motor_cmd_t;

typedef enum {
    ARM_STATE_DISARMED = 0,
    ARM_STATE_ARMED,
    ARM_STATE_FAILSAFE
} arm_state_t;

#endif
