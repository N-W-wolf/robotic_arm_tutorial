# robotic_tf_demo

第一阶段 TF 教学包：在 Black 比赛场地中演示 TF 树。

机械臂、平台和关节接口复用 `robotic_description` 包（单一数据源），
本包负责演示编排：

```text
MuJoCo 场景（scene_terrain.xml + robotic_arm.xml）
        │  qpos / xpos
        ▼
mujoco_scene_viewer.py ──/joint_states──▶ robot_state_publisher ──TF──▶ RViz2
        │                                          │
        └── map→base_footprint 动态 TF ────────────┴────▶ rqt_tf_tree
```

## 坐标与数据流

`map` 原点位于场地入口中心地面，x 轴沿场地左右方向，y 轴指向场地内部，
z 轴竖直向上。

- 8 个 `map → target1..8` 静态 TF：箱位地面中心（两排，y = 1.90 / 2.75 m）
- 动态 `map → base_footprint`：由 MuJoCo 平台位置实时发布，拖平台时跟随
- 静态 `base_footprint → base_link`：固定 0.15 m 安装偏移
- `base_link → link1..4`：`robot_state_publisher` 根据 URDF + `/joint_states` 生成
- `/joint_states`：viewer 从 MuJoCo `qpos` 桥接发布（position/velocity/effort）

在 MuJoCo 窗口里拖动机械臂关节，RViz2 中的模型与 TF 树实时跟随——
这是"TF = 正运动学"的直观演示。

## 启动

```bash
cd ~/WorkFile/robotic_tutorial/ros2_ws
source install/setup.bash
ros2 launch robotic_tf_demo tf_static.launch.py
```

该 launch 会同时启动：

- MuJoCo `scene_terrain.xml` 图形界面（场地、箱位、平台和机械臂，不加载机器狗）
- `mujoco_tf_bridge`：发布 `map → base_footprint`、`base_footprint → base_link`
  和 `/joint_states`
- 8 个 `map → targetN` 静态 TF
- `robot_state_publisher`（加载 robotic_description 的 URDF）
- RViz2（预配置 TF 显示，Fixed Frame = map）
- `rqt_tf_tree` TF 树界面

## 查看 TF 数值

TF 树只能看到坐标系关系，数值用 `tf2_echo`：

```bash
ros2 run tf2_ros tf2_echo base_link link4     # 末端相对基座
ros2 run tf2_ros tf2_echo map link1           # 任意两帧
```

- 平移单位米，RPY 单位弧度
- 拖动 MuJoCo 中的机械臂时数值实时刷新，Ctrl+C 退出
- Humble 版不支持 `--once`，只能持续监听

生成 TF 树 PDF 快照（含发布者与频率信息）：

```bash
ros2 run tf2_tools view_frames
```

## 依赖

```bash
python3 -m pip install mujoco   # MuJoCo Python 包
```

包依赖：`robotic_description`（URDF + MJCF 单一数据源）、
`robot_state_publisher`、`rviz2`、`rqt_tf_tree`。
