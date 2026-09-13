cmake_minimum_required(VERSION 3.8)
project(robotic_sldasm_ros2_humble)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY config launch meshes urdf rviz docs mujoco
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
