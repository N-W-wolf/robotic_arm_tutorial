import os

from ament_index_python.packages import (
    get_package_prefix,
    get_package_share_directory,
)
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch.substitutions import FindExecutable
from launch_ros.actions import Node


TARGETS = {
    "target1": (-1.275, 1.90, 0.0),
    "target2": (-0.425, 1.90, 0.0),
    "target3": (0.425, 1.90, 0.0),
    "target4": (1.275, 1.90, 0.0),
    "target5": (-1.275, 2.75, 0.0),
    "target6": (-0.425, 2.75, 0.0),
    "target7": (0.425, 2.75, 0.0),
    "target8": (1.275, 2.75, 0.0),
}


def static_target_node(name: str, xyz: tuple[float, float, float]) -> Node:
    return Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name=f"map_to_{name}",
        arguments=[
            "--x", str(xyz[0]),
            "--y", str(xyz[1]),
            "--z", str(xyz[2]),
            "--qx", "0.0",
            "--qy", "0.0",
            "--qz", "0.0",
            "--qw", "1.0",
            "--frame-id", "map",
            "--child-frame-id", name,
        ],
        output="screen",
    )


def generate_launch_description() -> LaunchDescription:
    tf_share = get_package_share_directory("robotic_tf_demo")
    desc_share = get_package_share_directory("robotic_description")

    viewer_path = os.path.join(
        get_package_prefix("robotic_tf_demo"),
        "lib",
        "robotic_tf_demo",
        "mujoco_scene_viewer.py",
    )
    urdf_path = os.path.join(desc_share, "urdf", "robotic.urdf")
    rviz_config = os.path.join(tf_share, "config", "tf_display.rviz")

    with open(urdf_path, encoding="utf-8") as urdf_file:
        robot_description = urdf_file.read()

    return LaunchDescription(
        [
            *[static_target_node(name, xyz) for name, xyz in TARGETS.items()],
            ExecuteProcess(
                cmd=[
                    FindExecutable(name="python3"),
                    "-u",
                    viewer_path,
                ],
                output="screen",
            ),
            Node(
                package="robot_state_publisher",
                executable="robot_state_publisher",
                name="robot_state_publisher",
                parameters=[{"robot_description": robot_description}],
                output="screen",
            ),
            Node(
                package="rviz2",
                executable="rviz2",
                name="rviz2",
                arguments=["-d", rviz_config],
                output="screen",
            ),
            Node(
                package="rqt_tf_tree",
                executable="rqt_tf_tree",
                name="rqt_tf_tree",
                output="screen",
            ),
        ]
    )
