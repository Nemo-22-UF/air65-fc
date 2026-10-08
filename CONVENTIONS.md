# Conventions

These are fixed. Most flight-controller bugs come from breaking one of them.

## Axes (body frame)

- x points forward, y points right, z points down.
- **+roll** = right side goes down.
- **+pitch** = nose goes up.
- **+yaw** = nose turns right (clockwise seen from above).

## Units

| Quantity | Unit |
| --- | --- |
| Angular rate | degrees per second (`_dps`) |
| Acceleration | g (`_g`) |
| Time step `dt` | seconds, `float` |
| Timestamps | microseconds, `uint32_t` (`_us`) |
| Sticks roll/pitch/yaw | -1.0 to +1.0 |
| Throttle | 0.0 to 1.0 |
| Motor commands | 0.0 to 1.0 (DShot values only inside `hw/dshot.c`) |

Put the unit in the variable name when it is not obvious: `rate_dps`, `now_us`.

## Motor order (simulator; verify on the real Air65)

```
        FRONT
   M4         M2
     \       /
      \     /
      /     \
     /       \
   M3         M1
        REAR
```

| Motor | Position | Spin (props-in assumption) |
| --- | --- | --- |
| M1 | rear right | clockwise |
| M2 | front right | counter-clockwise |
| M3 | rear left | counter-clockwise |
| M4 | front left | clockwise |

The mixer table lives in one place (`src/control/mixer.c`) so it can be changed for the real board.

## Code style

- C11. Compiles with zero warnings under `-Wall -Wextra`.
- Module prefix on every public function: `pid_`, `mixer_`, `safety_`, `imu_`.
- No global variables shared between modules. Pass structs.
- Check inputs at module boundaries: `dt <= 0`, NaN, out-of-range sticks.
