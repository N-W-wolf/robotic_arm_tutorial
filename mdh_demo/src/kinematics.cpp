#include "mdh/kinematics.hpp"

#include <algorithm>
#include <cmath>

namespace mdh {

Eigen::Isometry3d transform(const Parameter& parameter, double joint_angle) {
  Eigen::Isometry3d result = Eigen::Isometry3d::Identity();
  result = result *
           Eigen::AngleAxisd(parameter.alpha, Eigen::Vector3d::UnitX());
  result = result * Eigen::Translation3d(parameter.a, 0.0, 0.0);
  result = result *
           Eigen::AngleAxisd(joint_angle + parameter.theta_offset,
                             Eigen::Vector3d::UnitZ());
  result = result * Eigen::Translation3d(0.0, 0.0, parameter.d);
  return result;
}

Result forward(const std::array<Parameter, 4>& parameters,
               const std::array<double, 4>& joint_angles) {
  Result result;
  result.frames[0] = Eigen::Isometry3d::Identity();

  for (std::size_t i = 0; i < parameters.size(); ++i) {
    result.relative[i] = transform(parameters[i], joint_angles[i]);
    result.frames[i + 1] = result.frames[i] * result.relative[i];
  }

  return result;
}

Eigen::Vector3d rpy(const Eigen::Matrix3d& rotation) {
  // XYZ fixed-axis roll-pitch-yaw angles, useful for display only.
  const double pitch = std::asin(
      std::clamp(-rotation(2, 0), -1.0, 1.0));
  const double roll = std::atan2(rotation(2, 1), rotation(2, 2));
  const double yaw = std::atan2(rotation(1, 0), rotation(0, 0));
  return Eigen::Vector3d(roll, pitch, yaw);
}

}  // namespace mdh
