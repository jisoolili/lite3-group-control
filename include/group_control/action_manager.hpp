#ifndef GROUP_CONTROL_ACTION_MANAGER_HPP_
#define GROUP_CONTROL_ACTION_MANAGER_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "group_control/action.hpp"
#include "group_control/robot_manager.hpp"

namespace group_control {

class ActionManager {
 public:
  bool registerAction(std::unique_ptr<Action> action);
  bool hasAction(const std::string& action_name) const;
  Action* getAction(const std::string& action_name);
  const Action* getAction(const std::string& action_name) const;
  std::vector<std::string> actionNames() const;
  bool execute(Robot& robot, const std::string& action_name);
  bool execute(RobotManager& robot_manager,
               const std::string& robot_id,
               const std::string& action_name);

  static ActionManager createDefaultDryRunManager();

 private:
  std::unordered_map<std::string, std::unique_ptr<Action>> actions_;
};

}  // namespace group_control

#endif  // GROUP_CONTROL_ACTION_MANAGER_HPP_
