cmake_minimum_required(VERSION 3.8)
project(robotic_description)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY meshes urdf docs mujoco
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
