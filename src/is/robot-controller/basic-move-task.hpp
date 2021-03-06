
#pragma once

#include <is/msgs/common.pb.h>
#include <is/msgs/robot.pb.h>
#include <cmath>
#include "control-task.hpp"

namespace is {

class BasicMoveTask : public ControlTask {
  int target;
  std::vector<is::common::Position> positions;
  std::vector<is::common::Speed> speeds;
  double allowed_error;
  double frequency;
  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point end;

 public:
  BasicMoveTask(is::robot::BasicMoveTask const&);

  auto rate() const -> double override;

  auto done() const -> bool override;
  auto completion() const -> double override;

  void update(is::common::Pose const&);
  auto error(is::common::Pose const&) const -> double override;

  auto target_pose() const -> is::common::Pose override;
  auto target_speed() const -> is::common::Speed override;

  auto began_at() const -> std::chrono::system_clock::time_point override;
  auto ended_at() const -> std::chrono::system_clock::time_point override;
};

}  // namespace is