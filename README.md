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
