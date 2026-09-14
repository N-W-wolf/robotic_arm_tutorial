# MDH Demo

An independent C++17 teaching demo for modified Denavit-Hartenberg forward
kinematics. It does not depend on ROS 2, URDF, or MuJoCo.

## Features

- Eigen implementation of
  `A_i = Rx(alpha_(i-1)) Tx(a_(i-1)) Rz(theta_i) Tz(d_i)`
- Four `+Z` revolute joints matching the teaching convention
- Live joint-angle sliders
- 3D skeleton, links, joints, and coordinate frames
- End-effector position and fixed-axis RPY display
- Expandable `A1` through `A4` and `T04` matrices
- Grid and coordinate-frame toggles

## Build

The project expects Eigen 3.3 or newer. On Ubuntu, install GLFW and Dear ImGui
from the system package manager. If they are unavailable, CMake falls back to
downloading them through `FetchContent`.

On Ubuntu, the system packages are typically:

```bash
sudo apt install build-essential cmake libeigen3-dev libgl1-mesa-dev libglfw3-dev libimgui-dev
```

Then build:

```bash
cmake -S . -B build
cmake --build build -j
./build/mdh_demo
```

On a headless machine, set up an X11 display or virtual display before
launching the GUI.
