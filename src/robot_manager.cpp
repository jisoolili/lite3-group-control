#include "group_control/robot_manager.hpp"

namespace group_control {

bool RobotManager::addRobot(const RobotConfig& config) {
  if (config.robot_id.empty() || robots_.count(config.robot_id) != 0U) {
    return false;
  }
  robots_.emplace(config.robot_id, std::make_unique<Robot>(config));
  return true;
}

bool RobotManager::removeRobot(const std::string& robot_id) {
  return robots_.erase(robot_id) > 0U;
}

bool RobotManager::hasRobot(const std::string& robot_id) const {
  return robots_.count(robot_id) != 0U;
}

Robot* RobotManager::getRobot(const std::string& robot_id) {
  const auto it = robots_.find(robot_id);
  return it == robots_.end() ? nullptr : it->second.get();
}

const Robot* RobotManager::getRobot(const std::string& robot_id) const {
  const auto it = robots_.find(robot_id);
  return it == robots_.end() ? nullptr : it->second.get();
}

std::vector<Robot*> RobotManager::getAllRobots() {
  std::vector<Robot*> result;
  result.reserve(robots_.size());
  for (auto& entry : robots_) {
    result.push_back(entry.second.get());
  }
  return result;
}

std::vector<const Robot*> RobotManager::getAllRobots() const {
  std::vector<const Robot*> result;
  result.reserve(robots_.size());
  for (const auto& entry : robots_) {
    result.push_back(entry.second.get());
  }
  return result;
}

std::size_t RobotManager::size() const { return robots_.size(); }

}  // namespace group_control
