#ifndef GROUP_CONTROL_GROUP_CONTROLLER_HPP_
#define GROUP_CONTROL_GROUP_CONTROLLER_HPP_

#include <chrono>
#include <string>
#include <vector>

#include "group_control/action_manager.hpp"
#include "group_control/robot_manager.hpp"

namespace group_control {

struct ActionCommand {
  std::string robot_id;
  std::string action;
  double start_delay = 0.0;
};

using SequenceStep = ActionCommand;

struct DispatchRecord {
  std::size_t sequence_index = 0;
  std::string robot_id;
  std::string action;
  double planned_start = 0.0;
  double actual_dispatch = 0.0;
  double delay = 0.0;
  bool success = false;
};

class GroupController {
 public:
  GroupController(RobotManager& robot_manager, ActionManager& action_manager);

  std::vector<DispatchRecord> runOne(const std::string& robot_id,
                                     const std::string& action);
  std::vector<DispatchRecord> runGroup(const std::vector<std::string>& robot_ids,
                                       const std::string& action);
  std::vector<DispatchRecord> runWithDelay(const std::string& robot_id,
                                           const std::string& action,
                                           double delay_seconds);
  std::vector<DispatchRecord> runDifferentActions(
      const std::vector<ActionCommand>& commands);
  std::vector<DispatchRecord> runSequence(
      const std::vector<SequenceStep>& steps);

 private:
  std::vector<DispatchRecord> runPlannedCommands(
      const std::vector<ActionCommand>& commands);

  RobotManager& robot_manager_;
  ActionManager& action_manager_;
};

void printDispatchRecord(const DispatchRecord& record);

}  // namespace group_control

#endif  // GROUP_CONTROL_GROUP_CONTROLLER_HPP_
