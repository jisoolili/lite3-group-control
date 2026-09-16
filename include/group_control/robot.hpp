#ifndef GROUP_CONTROL_ROBOT_HPP_
#define GROUP_CONTROL_ROBOT_HPP_

#include <chrono>
#include <cstdint>
#include <string>

#include "group_control/robot_config.hpp"

namespace group_control {

enum class ConnectionState {
  kDisconnected,
  kConfigured,
  kMockConnected,
};

class Robot {
 public:
  explicit Robot(RobotConfig config);

  const std::string& robotId() const;
  const std::string& ip() const;
  std::uint16_t targetPort() const;
  std::uint16_t localStatePort() const;
  ConnectionState connectionState() const;
  std::chrono::steady_clock::time_point lastStateTime() const;

  void markMockConnected();
  void markDisconnected();
  void updateLastStateTime(std::chrono::steady_clock::time_point time);

  const RobotConfig& config() const;

 private:
  RobotConfig config_;
  ConnectionState connection_state_ = ConnectionState::kConfigured;
  std::chrono::steady_clock::time_point last_state_time_{};
};

}  // namespace group_control

#endif  // GROUP_CONTROL_ROBOT_HPP_
