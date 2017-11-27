#ifndef __FENCE_HPP__
#define __FENCE_HPP__

#include <is/is.hpp>
#include <is/msgs/common.pb.h>
#include <is/msgs/robot.pb.h>
#include <armadillo>

namespace fence {

using namespace is::robot;
using namespace is::common;

// arma::mat limit_pose(arma::vec const& current_pose, arma::vec const& desired_pose, arma::vec const& fence) {
//   auto x = current_pose(0);
//   auto y = current_pose(1);
//   auto xd = desired_pose(0);
//   auto yd = desired_pose(1);
//   auto x_min = fence(0);
//   auto x_max = fence(1);
//   auto y_min = fence(2);
//   auto y_max = fence(3);
//   auto a = (yd - y) / (xd - x);
//   auto b = 1 / a;
//   auto x_out = xd > x_max || xd < x_min;
//   auto y_out = yd > y_max || yd < y_min;
//   if (x_out || y_out) {
//     is::log::warn("Desired pose out of bounds. Limiting it.");
//   }
//   auto xd_b = x_out ? std::min(std::max(xd, x_min), x_max) : xd;
//   auto yd_b = y_out ? std::min(std::max(yd, y_min), y_max) : yd;
//   xd_b = !x_out && y_out ? b * (yd_b - y) + x : xd_b;
//   yd_b = x_out && !y_out ? a * (xd_b - x) + y : yd_b;
//   arma::mat new_desired_pose(desired_pose);
//   new_desired_pose(0) = xd_b;
//   new_desired_pose(1) = yd_b;
//   return new_desired_pose;
// }

// arma::mat limit_speed(arma::vec const& current_pose, arma::vec const& speed, arma::vec const& fence) {
//   if (current_pose.empty())
//     return arma::mat(2, 1, arma::fill::zeros);
//   auto x = current_pose(0);
//   auto y = current_pose(1);
//   auto x_min = fence(0);
//   auto x_max = fence(1);
//   auto y_min = fence(2);
//   auto y_max = fence(3);
//   auto x_out = x > x_max || x < x_min;
//   auto y_out = y > y_max || y < y_min;
//   if (x_out || y_out) {
//     is::log::warn("Robot leaving critical area. Forcing stop.");
//     return arma::mat(2, 1, arma::fill::zeros);
//   }
//   return speed;
// }

// Speed limit_speed(arma::vec const& current_pose, Speed speed, arma::vec const& fence) {
//   arma::vec speed_vec = limit_speed(current_pose, arma::vec({speed.linear, speed.angular}), fence);
//   speed.linear = speed_vec(0);
//   speed.angular = speed_vec(1);
//   return speed;
// }

Speed limit_speed(RobotControllerProgress const& status, arma::vec const& fence) {
  Speed speed;
  if (!status.has_current_pose())
    return speed;
  auto x = status.current_pose().position().x();
  auto y = status.current_pose().position().y();
  auto x_min = fence(0);
  auto x_max = fence(1);
  auto y_min = fence(2);
  auto y_max = fence(3);
  auto x_out = x > x_max || x < x_min;
  auto y_out = y > y_max || y < y_min;
  if (x_out || y_out) {
    is::warn("Robot leaving critical area. Forcing stop.");
    return speed;
  }
  return status.current_speed();
}

}  // ::fence

#endif  // __FENCE_HPP__