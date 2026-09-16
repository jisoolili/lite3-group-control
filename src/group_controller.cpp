#include "group_control/group_controller.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

namespace group_control {
namespace {

double SecondsSince(std::chrono::steady_clock::time_point start) {
  return std::chrono::duration<double>(std::chrono::steady_clock::now() - start)
      .count();
}

}  // namespace

GroupController::GroupController(RobotManager& robot_manager,
                                 ActionManager& action_manager)
    : robot_manager_(robot_manager), action_manager_(action_manager) {}

std::vector<DispatchRecord> GroupController::runOne(
    const std::string& robot_id, const std::string& action) {
  return runPlannedCommands({ActionCommand{robot_id, action, 0.0}});
}

std::vector<DispatchRecord> GroupController::runGroup(
    const std::vector<std::string>& robot_ids, const std::string& action) {
  std::vector<ActionCommand> commands;
  commands.reserve(robot_ids.size());
  for (const auto& robot_id : robot_ids) {
    commands.push_back(ActionCommand{robot_id, action, 0.0});
  }
  return runPlannedCommands(commands);
}

std::vector<DispatchRecord> GroupController::runWithDelay(
    const std::string& robot_id, const std::string& action,
    const double delay_seconds) {
  return runPlannedCommands({ActionCommand{robot_id, action, delay_seconds}});
}

std::vector<DispatchRecord> GroupController::runDifferentActions(
    const std::vector<ActionCommand>& commands) {
  return runPlannedCommands(commands);
}

std::vector<DispatchRecord> GroupController::runSequence(
    const std::vector<SequenceStep>& steps) {
  return runPlannedCommands(steps);
}

std::vector<DispatchRecord> GroupController::runPlannedCommands(
    const std::vector<ActionCommand>& commands) {
  std::vector<ActionCommand> sorted = commands;
  std::stable_sort(sorted.begin(), sorted.end(),
                   [](const ActionCommand& lhs, const ActionCommand& rhs) {
                     return lhs.start_delay < rhs.start_delay;
                   });

  std::cout << "[GROUP START]\n";
  const auto start = std::chrono::steady_clock::now();
  std::vector<DispatchRecord> records;
  records.reserve(sorted.size());

  for (std::size_t i = 0; i < sorted.size(); ++i) {
    const ActionCommand& command = sorted[i];
    const auto planned_time =
        start + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::duration<double>(command.start_delay));
    std::this_thread::sleep_until(planned_time);

    DispatchRecord record;
    record.sequence_index = i + 1;
    record.robot_id = command.robot_id;
    record.action = command.action;
    record.planned_start = command.start_delay;
    record.actual_dispatch = SecondsSince(start);
    record.delay = command.start_delay;

    Robot* robot = robot_manager_.getRobot(command.robot_id);
    if (robot == nullptr) {
      std::cout << "[DISPATCH_ERROR] robot=" << command.robot_id
                << " action=" << command.action << " reason=unknown_robot\n";
      record.success = false;
    } else {
      record.success = action_manager_.execute(*robot, command.action);
    }
    printDispatchRecord(record);
    records.push_back(record);
  }

  std::cout << "[GROUP END]\n";
  return records;
}

void printDispatchRecord(const DispatchRecord& record) {
  const auto old_precision = std::cout.precision();
  const auto old_flags = std::cout.flags();
  std::cout << std::fixed << std::setprecision(3);
  std::cout << "[" << std::setw(3) << std::setfill('0')
            << record.sequence_index << std::setfill(' ') << "] "
            << record.robot_id << " " << record.action << "\n"
            << "planned=" << record.planned_start << "\n"
            << "dispatch=" << record.actual_dispatch << "\n"
            << "delay=" << record.delay << "\n"
            << "success=" << (record.success ? "true" : "false") << "\n";
  std::cout.flags(old_flags);
  std::cout.precision(old_precision);
}

}  // namespace group_control
