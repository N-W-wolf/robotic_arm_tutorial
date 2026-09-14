#include "mdh/renderer.hpp"

#include <algorithm>
#include <cmath>

#include <GL/gl.h>

namespace {

constexpr double kPi = 3.14159265358979323846;

void perspective(double field_of_view,
                 double aspect,
                 double near_plane,
                 double far_plane) {
  const double top =
      near_plane * std::tan(field_of_view * 0.5 * kPi / 180.0);
  const double right = top * aspect;
  glFrustum(-right, right, -top, top, near_plane, far_plane);
}

}  // namespace

namespace mdh {

void Renderer::draw(const std::array<Eigen::Isometry3d, 5>& frames,
                    int width,
                    int height,
                    bool show_grid,
                    bool show_frames) const {
  const int safe_height = std::max(height, 1);
  glViewport(0, 0, width, safe_height);
  glClearColor(0.035f, 0.045f, 0.065f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  perspective(45.0, static_cast<double>(width) / safe_height, 0.01, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Front teaching view: world X is horizontal and world Z is vertical.
  glTranslatef(-0.18f, -0.06f, -0.82f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

  if (show_grid) {
    draw_grid();
  }

  for (std::size_t i = 0; i + 1 < frames.size(); ++i) {
    const Eigen::Vector3d start = frames[i].translation();
    const Eigen::Vector3d end = frames[i + 1].translation();
    draw_line(start, end, 0.82f, 0.86f, 0.95f, 7.0f);
  }

  for (const auto& frame : frames) {
    draw_joint(frame.translation());
    if (show_frames) {
      draw_axes(frame, 0.055f);
    }
  }

  glDisable(GL_DEPTH_TEST);
}

void Renderer::draw_axes(const Eigen::Isometry3d& frame, float length) {
  const Eigen::Vector3d origin = frame.translation();
  const Eigen::Matrix3d rotation = frame.rotation();
  const std::array<Eigen::Vector3d, 3> ends = {
      origin + length * rotation.col(0),
      origin + length * rotation.col(1),
      origin + length * rotation.col(2),
  };
  draw_line(origin, ends[0], 0.92f, 0.18f, 0.18f, 3.0f);
  draw_line(origin, ends[1], 0.20f, 0.85f, 0.30f, 3.0f);
  draw_line(origin, ends[2], 0.20f, 0.48f, 1.00f, 3.0f);
}

void Renderer::draw_line(const Eigen::Vector3d& start,
                         const Eigen::Vector3d& end,
                         float red,
                         float green,
                         float blue,
                         float width) {
  glLineWidth(width);
  glColor3f(red, green, blue);
  glBegin(GL_LINES);
  glVertex3d(start.x(), start.y(), start.z());
  glVertex3d(end.x(), end.y(), end.z());
  glEnd();
}

void Renderer::draw_joint(const Eigen::Vector3d& position) {
  glPointSize(10.0f);
  glColor3f(1.0f, 0.78f, 0.18f);
  glBegin(GL_POINTS);
  glVertex3d(position.x(), position.y(), position.z());
  glEnd();
}

void Renderer::draw_grid() {
  glLineWidth(1.0f);
  glColor3f(0.16f, 0.19f, 0.24f);
  glBegin(GL_LINES);
  // A quiet X-Z guide plane behind the arm, rather than a perspective floor.
  constexpr double y = 0.03;
  for (int i = -10; i <= 16; ++i) {
    const double x = static_cast<double>(i) * 0.05;
    glVertex3d(x, y, 0.0);
    glVertex3d(x, y, 0.55);
  }
  for (int i = 0; i <= 11; ++i) {
    const double z = static_cast<double>(i) * 0.05;
    glVertex3d(-0.5, y, z);
    glVertex3d(0.8, y, z);
  }
  glEnd();
}

}  // namespace mdh
