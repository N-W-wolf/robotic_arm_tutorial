# robotic_sldasm_ros2_humble

ROS 2 Humble teaching package for the four-joint arm exported from
SolidWorks.

The original SolidWorks export is kept in the sibling directory
`robotic.SLDASM`. This package is the ROS 2 teaching copy.

## Display in RViz2

```bash
colcon build --symlink-install
source install/setup.bash
ros2 launch robotic_sldasm_ros2_humble display.launch.py
```

The GUI publishes positions for `joint1` through `joint4`.

## Teaching conventions

All four revolute joint axes are `0 0 1`. This makes positive joint position
and positive MDH `theta` use the same sign. Consequently, positive commands
for `joint2`, `joint3`, and `joint4` move in the opposite direction from the
unchanged SolidWorks-exported URDF, whose axes for those joints are `0 0 -1`.

The limits and effort/velocity values in the URDF are didactic defaults:

```text
joint1: [-pi, pi],       effort = 10.0, velocity = 2.0
joint2: [-pi/2, pi/2],   effort = 10.0, velocity = 2.0
joint3: [-pi/2, pi/2],   effort = 10.0, velocity = 2.0
joint4: [-pi/2, pi/2],   effort = 10.0, velocity = 2.0
```

They should be replaced with measured mechanical and actuator limits before
using the model for hardware or dynamics studies.

## MuJoCo torque model

`mujoco/robotic_sldasm_torque.xml` is an ideal direct-torque model. Its four
motor controls are joint torques in N*m because each actuator has `gear="1"`.
Joint damping, friction loss, and armature are explicitly set to zero.
