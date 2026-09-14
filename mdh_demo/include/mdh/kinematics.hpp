#pragma once

#include <array>

#include <Eigen/Geometry>

namespace mdh {

struct Parameter {
  double a;
  double alpha;
  double d;
  double theta_offset;
};

struct Result {
  std::array<Eigen::Isometry3d, 5> frames;
  std::array<Eigen::Isometry3d, 4> relative;
};

Eigen::Isometry3d transform(const Parameter& parameter, double joint_angle);
Result forward(const std::array<Parameter, 4>& parameters,
               const std::array<double, 4>& joint_angles);
Eigen::Vector3d rpy(const Eigen::Matrix3d& rotation);

}  // namespace mdh
