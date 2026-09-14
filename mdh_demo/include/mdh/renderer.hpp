#pragma once

#include <array>

#include <Eigen/Geometry>

namespace mdh {

class Renderer {
 public:
  void draw(const std::array<Eigen::Isometry3d, 5>& frames,
            int width,
            int height,
            bool show_grid,
            bool show_frames) const;

 private:
  static void draw_axes(const Eigen::Isometry3d& frame, float length);
  static void draw_line(const Eigen::Vector3d& start,
                        const Eigen::Vector3d& end,
                        float red,
                        float green,
                        float blue,
                        float width);
  static void draw_joint(const Eigen::Vector3d& position);
  static void draw_grid();
};

}  // namespace mdh
