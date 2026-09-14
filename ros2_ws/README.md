# robotic_tutorial ROS 2 workspace

This workspace contains the ROS 2 packages for the robotic tutorial project.

## Build

```bash
cd ~/WorkFile/robotic_tutorial/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --symlink-install
source install/setup.bash
```

ROS 1 packages and non-ROS teaching materials are kept outside `src`:

```text
../legacy_ros1/   ROS 1 catkin package
../standalone/    independent CMake teaching programs
../cad/           CAD source files
../matlab/        Matlab demonstrations
```
