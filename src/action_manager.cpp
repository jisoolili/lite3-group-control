#include "group_control/action_manager.hpp"

#include <iostream>
#include <utility>

namespace group_control {

Lite3StandDemoDryRunAction::Lite3StandDemoDryRunAction(
    std::string name, std::string lite3_action)
    : name_(std::move(name)), lite3_action_(std::move(lite3_action)) {}

const std::string& Lite3StandDemoDryRunAction::name() const { return name_; }

const std::string& Lite3StandDemoDryRunAction::lite3Action() const {
  return lite3_action_;
}

const std::string& Lite3StandDemoDryRunAction::lastDryRunCommand() const {
  return last_dry_run_command_;
}

bool Lite3StandDemoDryRunAction::execute(Robot& robot) {
  last_dry_run_command_ =
      "ros2 run transfer lite3_stand_demo --dry-run --action " + lite3_action_;
  std::cout << "[DRY RUN]\n"
            << "robot=" << robot.robotId() << "\n"
            << "ip=" << robot.ip() << "\n"
            << "target_port=" << robot.targetPort() << "\n"
            << "local_state_port=" << robot.localStatePort() << "\n"
            << "action=" << name_ << "\n"
            << "lite3_action=" << lite3_action_ << "\n"
            << "command=" << last_dry_run_command_ << "\n"
            << "NO COMMAND SENT TO ROBOT\n";
  robot.markMockConnected();
  return true;
}

bool ActionManager::registerAction(std::unique_ptr<Action> action) {
  if (!action || action->name().empty() || actions_.count(action->name()) != 0U) {
    return false;
  }
  const std::string key = action->name();
  actions_.emplace(key, std::move(action));
  return true;
}

bool ActionManager::hasAction(const std::string& action_name) const {
  return actions_.count(action_name) != 0U;
}

Action* ActionManager::getAction(const std::string& action_name) {
  const auto it = actions_.find(action_name);
  return it == actions_.end() ? nullptr : it->second.get();
}

const Action* ActionManager::getAction(const std::string& action_name) const {
  const auto it = actions_.find(action_name);
  return it == actions_.end() ? nullptr : it->second.get();
}

std::vector<std::string> ActionManager::actionNames() const {
  std::vector<std::string> names;
  names.reserve(actions_.size());
  for (const auto& entry : actions_) {
    names.push_back(entry.first);
  }
  return names;
}

bool ActionManager::execute(Robot& robot, const std::string& action_name) {
  Action* action = getAction(action_name);
  return action != nullptr && action->execute(robot);
}

bool ActionManager::execute(RobotManager& robot_manager,
                            const std::string& robot_id,
                            const std::string& action_name) {
  Robot* robot = robot_manager.getRobot(robot_id);
  return robot != nullptr && execute(*robot, action_name);
}

ActionManager ActionManager::createDefaultDryRunManager() {
  ActionManager manager;
  manager.registerAction(
      std::make_unique<Lite3StandDemoDryRunAction>("stand", "stand"));
  manager.registerAction(
      std::make_unique<Lite3StandDemoDryRunAction>("squat", "squat-sequence"));
  manager.registerAction(
      std::make_unique<Lite3StandDemoDryRunAction>("squat_low", "squat-1-3"));
  manager.registerAction(
      std::make_unique<Lite3StandDemoDryRunAction>("squat_mid", "squat-2-3"));
  manager.registerAction(
      std::make_unique<Lite3StandDemoDryRunAction>("squat_high", "squat-full"));
  return manager;
}

}  // namespace group_control
