cmake_minimum_required(VERSION 3.20)
project(mdh_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 QUIET)

set(IMGUI_ROOT "" CACHE PATH "Root prefix containing libimgui-dev files")
if(NOT IMGUI_ROOT)
  set(IMGUI_ROOT "/")
endif()

set(IMGUI_INCLUDE_DIR "${IMGUI_ROOT}/usr/include/imgui")
set(IMGUI_LIBRARY "${IMGUI_ROOT}/usr/lib/x86_64-linux-gnu/libimgui.a")
set(IMGUI_BACKENDS_DIR
  "${IMGUI_ROOT}/usr/share/doc/libimgui-dev/examples/backends")
set(STB_LIBRARY "${IMGUI_ROOT}/usr/lib/x86_64-linux-gnu/libstb.a")
set(HAVE_SYSTEM_IMGUI FALSE)
if(EXISTS "${IMGUI_INCLUDE_DIR}/imgui.h"
   AND EXISTS "${IMGUI_LIBRARY}"
   AND EXISTS "${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp"
   AND EXISTS "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl2.cpp")
  set(HAVE_SYSTEM_IMGUI TRUE)
endif()

if(NOT glfw3_FOUND OR NOT HAVE_SYSTEM_IMGUI)
  include(FetchContent)
endif()

if(NOT glfw3_FOUND)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(glfw)
endif()

if(HAVE_SYSTEM_IMGUI)
  add_library(imgui STATIC
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl2.cpp
  )

  target_include_directories(imgui PUBLIC
    ${IMGUI_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}/backends
    ${IMGUI_BACKENDS_DIR}
  )

  target_link_libraries(imgui PUBLIC
    ${IMGUI_LIBRARY}
    ${STB_LIBRARY}
    glfw
    OpenGL::GL
  )
else()
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(imgui)

  add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
  )

  target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
  )

  target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
endif()

add_executable(mdh_demo
  src/main.cpp
  src/kinematics.cpp
  src/renderer.cpp
)

target_include_directories(mdh_demo PRIVATE include)
target_link_libraries(mdh_demo PRIVATE imgui Eigen3::Eigen OpenGL::GL)

if(UNIX AND NOT APPLE)
  target_link_libraries(mdh_demo PRIVATE dl)
endif()

if(MSVC)
  target_compile_options(mdh_demo PRIVATE /W4)
else()
  target_compile_options(mdh_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()
