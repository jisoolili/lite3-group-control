#include "group_control/robot.hpp"

namespace group_control {

Robot::Robot(RobotConfig config) : config_(std::move(config)) {}

const std::string& Robot::robotId() const { return config_.robot_id; }

const std::string& Robot::ip() const { return config_.ip; }

std::uint16_t Robot::targetPort() const { return config_.target_port; }

std::uint16_t Robot::localStatePort() const { return config_.local_state_port; }

ConnectionState Robot::connectionState() const { return connection_state_; }

std::chrono::steady_clock::time_point Robot::lastStateTime() const {
  return last_state_time_;
}

void Robot::markMockConnected() {
  connection_state_ = ConnectionState::kMockConnected;
  updateLastStateTime(std::chrono::steady_clock::now());
}

void Robot::markDisconnected() {
  connection_state_ = ConnectionState::kDisconnected;
}

void Robot::updateLastStateTime(std::chrono::steady_clock::time_point time) {
  last_state_time_ = time;
}

const RobotConfig& Robot::config() const { return config_; }

}  // namespace group_control
