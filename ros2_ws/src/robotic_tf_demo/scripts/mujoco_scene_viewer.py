#!/usr/bin/env python3

import os
import time

import mujoco
import mujoco.viewer
from ament_index_python.packages import get_package_share_directory
import rclpy
from geometry_msgs.msg import TransformStamped
from rclpy.node import Node
from sensor_msgs.msg import JointState
from tf2_ros import StaticTransformBroadcaster, TransformBroadcaster


MAP_WORLD_Y_OFFSET = 0.5
BASE_LINK_Z_OFFSET = 0.15

ARM_JOINTS = ["joint1", "joint2", "joint3", "joint4"]
ARM_ACTUATORS = ["tau_joint1", "tau_joint2", "tau_joint3", "tau_joint4"]


def load_scene_with_arm() -> mujoco.MjModel:
    """Load the scene with the shared arm model include path injected."""
    tf_share = get_package_share_directory("robotic_tf_demo")
    desc_share = get_package_share_directory("robotic_description")
    scene_path = os.path.join(
        tf_share, "mujoco", "black_task", "scene_terrain.xml"
    )
    arm_path = os.path.join(desc_share, "mujoco", "robotic_arm.xml")

    with open(scene_path, encoding="utf-8") as scene_file:
        scene_xml = scene_file.read()
    if "__ARM_MODEL_PATH__" not in scene_xml:
        raise RuntimeError(
            "scene_terrain.xml is missing the __ARM_MODEL_PATH__ placeholder"
        )
    scene_xml = scene_xml.replace("__ARM_MODEL_PATH__", arm_path)

    # Scene assets use relative paths; resolve them against the scene dir.
    # The arm model's own relative paths resolve against the arm file itself.
    cwd = os.getcwd()
    os.chdir(os.path.dirname(scene_path))
    try:
        return mujoco.MjModel.from_xml_string(scene_xml)
    finally:
        os.chdir(cwd)


def publish_base_transforms(
    node: Node,
    dynamic_broadcaster: TransformBroadcaster,
    base_body_id: int,
    data: mujoco.MjData,
) -> None:
    now = node.get_clock().now().to_msg()
    base_tf = TransformStamped()
    base_tf.header.stamp = now
    base_tf.header.frame_id = "map"
    base_tf.child_frame_id = "base_footprint"
    base_tf.transform.translation.x = float(data.xpos[base_body_id][0])
    base_tf.transform.translation.y = float(data.xpos[base_body_id][1] + MAP_WORLD_Y_OFFSET)
    base_tf.transform.translation.z = float(data.xpos[base_body_id][2])
    # Publish the true orientation read from MuJoCo.
    # MuJoCo quaternions are wxyz; geometry_msgs/Quaternion is xyzw.
    quat = data.xquat[base_body_id]
    base_tf.transform.rotation.x = float(quat[1])
    base_tf.transform.rotation.y = float(quat[2])
    base_tf.transform.rotation.z = float(quat[3])
    base_tf.transform.rotation.w = float(quat[0])
    dynamic_broadcaster.sendTransform(base_tf)


def main() -> None:
    rclpy.init()
    node = Node("mujoco_tf_bridge")
    dynamic_broadcaster = TransformBroadcaster(node)
    static_broadcaster = StaticTransformBroadcaster(node)
    joint_state_publisher = node.create_publisher(JointState, "joint_states", 10)

    base_link_tf = TransformStamped()
    base_link_tf.header.stamp = node.get_clock().now().to_msg()
    base_link_tf.header.frame_id = "base_footprint"
    base_link_tf.child_frame_id = "base_link"
    base_link_tf.transform.translation.z = BASE_LINK_Z_OFFSET
    base_link_tf.transform.rotation.w = 1.0
    static_broadcaster.sendTransform(base_link_tf)

    model = load_scene_with_arm()
    data = mujoco.MjData(model)
    base_body_id = mujoco.mj_name2id(
        model, mujoco.mjtObj.mjOBJ_BODY, "base_footprint"
    )
    if base_body_id < 0:
        raise RuntimeError("MuJoCo scene does not contain base_footprint")

    joint_ids = {
        name: mujoco.mj_name2id(model, mujoco.mjtObj.mjOBJ_JOINT, name)
        for name in ARM_JOINTS
    }
    actuator_ids = {
        name: mujoco.mj_name2id(model, mujoco.mjtObj.mjOBJ_ACTUATOR, name)
        for name in ARM_ACTUATORS
    }
    missing = [
        name
        for name, idx in {**joint_ids, **actuator_ids}.items()
        if idx < 0
    ]
    if missing:
        raise RuntimeError(f"robotic_arm model is missing: {missing}")

    def publish_joint_states(now) -> None:
        """Mirror the MuJoCo arm joint state into ROS.

        This is the bridge that turns the shared robotic_arm.xml interface
        into the /joint_states expected by robot_state_publisher.
        """
        msg = JointState()
        msg.header.stamp = now
        msg.name = ARM_JOINTS
        msg.position = [
            float(data.qpos[model.jnt_qposadr[joint_ids[name]]])
            for name in ARM_JOINTS
        ]
        msg.velocity = [
            float(data.qvel[model.jnt_dofadr[joint_ids[name]]])
            for name in ARM_JOINTS
        ]
        msg.effort = [
            float(data.actuator_force[actuator_ids[name]])
            for name in ARM_ACTUATORS
        ]
        joint_state_publisher.publish(msg)

    try:
        with mujoco.viewer.launch_passive(model, data) as viewer:
            while viewer.is_running() and rclpy.ok():
                step_start = time.perf_counter()
                # Step the physics so mouse-drag perturbations (forces applied
                # to non-mocap bodies) are integrated and take effect.
                # The scene has normal gravity; the unactuated arm hangs under
                # its own weight until a controller takes over, and the boxes
                # rest on the floor. The mocap platform is unaffected.
                mujoco.mj_step(model, data)
                now = node.get_clock().now().to_msg()
                publish_base_transforms(node, dynamic_broadcaster, base_body_id, data)
                publish_joint_states(now)
                viewer.sync()
                remaining = model.opt.timestep - (time.perf_counter() - step_start)
                if remaining > 0:
                    time.sleep(remaining)
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
