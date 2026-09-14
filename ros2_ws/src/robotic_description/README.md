# robotic_description

整个 robotic_tutorial 项目的机器人描述资产包（robot description）。

URDF、STL 网格、MuJoCo 模型都只在本包定义一次，其余包（如
`robotic_tf_demo`、后续的机械臂控制包）通过依赖复用，不自行拷贝。

## 目录结构

```text
urdf/     robotic.urdf          机械臂描述（robot_state_publisher 用它发布关节链 TF）
meshes/   各连杆 STL 网格（URDF 与 MuJoCo 共用）
mujoco/
  robotic_arm.xml      单一数据源：mocap 平台 + base_link..link4 关节链
                       + joint1..joint4 关节接口 + 4 个力矩执行器
  robotic_torque.xml   独立直接力矩仿真（include robotic_arm.xml + 地板/灯光/天空）
docs/     mdh_parameters.md     MDH 参数表
```

## 关节接口（后续控制包对接约定）

- 关节名：`joint1`~`joint4`，轴全为 `+Z`（与 MDH `theta` 正方向一致）
- 执行器名：`tau_joint1`~`tau_joint4`，`gear="1"`，ctrl 即关节力矩（N·m），
  ctrlrange/forcerange 均为 ±10
- 关节阻尼为教学默认值 0.05；frictionloss、armature 为 0
- URDF 与 MJCF 的限位一致：

```text
joint1: [-pi, pi],        effort = 10.0, velocity = 2.0
joint2: [0, pi],          effort = 10.0, velocity = 2.0
joint3: [-4pi/5, 4pi/5],  effort = 10.0, velocity = 2.0
joint4: [-2pi/3, 2pi/3],  effort = 10.0, velocity = 2.0
```

用于真实硬件或动力学研究前，应替换为实测的机械与执行器限位。

## 常用命令

### 构建

```bash
cd ros2_ws
source /opt/ros/humble/setup.bash
colcon build --symlink-install
source install/setup.bash
```

只构建本包：

```bash
colcon build --symlink-install --packages-select robotic_description
```

### 单独查看 MuJoCo 力矩模型

```bash
python3 -m pip install mujoco   # 需要 MuJoCo Python 包
python3 -m mujoco.viewer --mjcf \
  $(ros2 pkg prefix robotic_description)/share/robotic_description/mujoco/robotic_torque.xml
```

### 配合 TF 演示

本包不直接启动 GUI。RViz2 的 TF 演示（MuJoCo 场景 + `/joint_states` 桥 +
`robot_state_publisher` + RViz）由 `robotic_tf_demo` 编排：

```bash
ros2 launch robotic_tf_demo tf_static.launch.py
```

查看关节链 TF 数值：

```bash
ros2 run tf2_ros tf2_echo base_link link4
```

## 教学约定

四个转动关节的轴全部设为 `0 0 1`，使正关节角与 MDH 的 `theta` 正方向一致。
因此对 `joint2`、`joint3`、`joint4` 的正向指令，与未改动的 SolidWorks
导出 URDF（这些关节轴为 `0 0 -1`）运动方向相反。原始导出包保留在
`legacy_ros1/robotic.SLDASM`，未做改动。

MDH 参数表见 `docs/mdh_parameters.md`。

