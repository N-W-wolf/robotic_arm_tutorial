cmake_minimum_required(VERSION 3.8)
project(robotic_tf_demo)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY launch mujoco config
  DESTINATION share/${PROJECT_NAME}
)

install(
  PROGRAMS scripts/mujoco_scene_viewer.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
