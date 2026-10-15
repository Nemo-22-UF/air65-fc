#include "mixer.h"

#include <math.h>
#include <stddef.h>

/*
 * Quad-X mixing table. One row per motor: {roll, pitch, yaw} factor.
 * Signs follow CONVENTIONS.md:
 *   +roll  = right side down -> left motors speed up
 *   +pitch = nose up         -> front motors speed up
 *   +yaw   = nose right (CW) -> counter-clockwise motors speed up
 *
 *        FRONT
 *   M4         M2
 *   M3         M1
 *        REAR
 *
 * This is the only place motor order lives; change it here for the real board.
 */
static const float MIX_TABLE[NUM_MOTORS][3] = {
    /*            roll   pitch   yaw  */
    /* M1 RR CW  */ { -1.0f, -1.0f, -1.0f },
    /* M2 FR CCW */ { -1.0f, +1.0f, +1.0f },
    /* M3 RL CCW */ { +1.0f, -1.0f, +1.0f },
    /* M4 FL CW  */ { +1.0f, +1.0f, -1.0f },
};

/* Replace NaN/inf with 0 and clamp to [lo, hi]. */
static float mixer_sanitize(float v, float lo, float hi)
{
    if (!isfinite(v)) {
        return 0.0f;
    }
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

void mixer_update(float throttle, float roll, float pitch, float yaw, motor_cmd_t *out)
{
    if (out == NULL) {
        return;
    }

    throttle = mixer_sanitize(throttle, 0.0f, 1.0f);
    roll = mixer_sanitize(roll, -1.0f, 1.0f);
    pitch = mixer_sanitize(pitch, -1.0f, 1.0f);
    yaw = mixer_sanitize(yaw, -1.0f, 1.0f);

    /* 1. Mix: throttle plus each correction times its sign for this motor. */
    float max_out = 0.0f;
    for (int i = 0; i < NUM_MOTORS; i++) {
        out->m[i] = throttle
                  + MIX_TABLE[i][0] * roll
                  + MIX_TABLE[i][1] * pitch
                  + MIX_TABLE[i][2] * yaw;
        if (i == 0 || out->m[i] > max_out) {
            max_out = out->m[i];
        }
    }

    /* 2. Airmode-lite: if any motor is above 1.0, shift all down equally
     *    so the differences between motors (the steering) are kept. */
    if (max_out > 1.0f) {
        float shift = max_out - 1.0f;
        for (int i = 0; i < NUM_MOTORS; i++) {
            out->m[i] -= shift;
        }
    }

    /* 3. Final clamp to the valid motor range 0..1. */
    for (int i = 0; i < NUM_MOTORS; i++) {
        out->m[i] = mixer_sanitize(out->m[i], 0.0f, 1.0f);
    }
}