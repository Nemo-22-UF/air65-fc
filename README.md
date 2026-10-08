# Air65 Flight Controller

Our own flight-controller firmware for an Air65-class 1S whoop, written in C.
We build it first as a PC simulator, then move it to the real STM32G473 board.

Full plan, task specs, and timeline: see the team guide (link in the group chat).

## Build and run

```
cmake -S . -B build
cmake --build build
./build/air65_sim
./build/air65_tests
```

On Windows the binaries are `build\Debug\air65_sim.exe` and `build\Debug\air65_tests.exe` if you use Visual Studio's generator.

Expected first output:

```
=============================
       AIR65 SIM BOOT OK
=============================
```

`air65_tests` fails on purpose until the mixer (Task 2) is written.

## Layout

| Folder | What lives there |
| --- | --- |
| `include/` | Shared types and hardware interfaces. Frozen; changes need team agreement |
| `src/control/` | PID, rates, mixer |
| `src/safety/` | Arming and failsafe |
| `src/estimation/` | Attitude estimation (later) |
| `sim/` | PC-only fakes: time, IMU, receiver, motors, physics |
| `hw/` | STM32 drivers (hardware phase, not created yet) |
| `tests/` | Unit tests |
| `tools/` | Python tools (log plotter) |
| `docs/` | Betaflight backup and notes |

Read `CONVENTIONS.md` before writing any control code.
