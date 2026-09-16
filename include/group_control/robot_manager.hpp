#ifndef GROUP_CONTROL_ROBOT_MANAGER_HPP_
#define GROUP_CONTROL_ROBOT_MANAGER_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "group_control/robot.hpp"

namespace group_control {

class RobotManager {
 public:
  bool addRobot(const RobotConfig& config);
  bool removeRobot(const std::string& robot_id);
  bool hasRobot(const std::string& robot_id) const;
  Robot* getRobot(const std::string& robot_id);
  const Robot* getRobot(const std::string& robot_id) const;
  std::vector<Robot*> getAllRobots();
  std::vector<const Robot*> getAllRobots() const;
  std::size_t size() const;

 private:
  std::unordered_map<std::string, std::unique_ptr<Robot>> robots_;
};

}  // namespace group_control

#endif  // GROUP_CONTROL_ROBOT_MANAGER_HPP_
