# VibeSim

This project implements a 2D population simulation in C++ using SFML. Multiple individuals navigate a bounded space, follow path segments, avoid collisions, and interact with circular goal areas. The system combines geometric reasoning, discrete collision grids, and segment-based navigation.

## Build Instructions

This project uses CMake (minimum version 3.20) and requires SFML 2.6 with the graphics, window, and system components.

### Requirements

* CMake ≥ 3.20
* C++17 compatible compiler
* SFML 2.6 installed on your system

Make sure SFML is discoverable by CMake (either installed system-wide or via CMAKE_PREFIX_PATH).

### Build Steps (Linux / macOS / Windows)

From the project root directory:
```
mkdir build
cd build
cmake ..
cmake --build .
```
This will generate the executable:
```
VibeSim
```

## Automated test mode

For performance measurements and reproducible scenarios, the binary accepts an
`--autotest` mode that builds a deterministic scenario (space, obstacles,
start/goal zones, bio-inspired route network and population) and reports FPS
and per-frame timings while saving screenshots:

```
./VibeSim --autotest --n 2000 --seconds 10 --out evidencias/bench_2000
```

Options: `--n <individuals>` (population to place), `--seconds <duration>`,
`--out <file prefix>` (writes `<prefix>_0_escenario.png`,
`<prefix>_1_simulacion.png`, `<prefix>_2_final.png` and prints per-second FPS
reports plus a final summary to stdout). Normal interactive use is unaffected
(only enabled with `--autotest`).
