#include <iostream>
#include <cmath>
#include <string>

#include "group_control/action_manager.hpp"
#include "group_control/group_controller.hpp"
#include "group_control/robot_manager.hpp"

namespace {

int failures = 0;

void Check(const bool condition, const std::string& label) {
  if (condition) {
    std::cout << label << ": PASS\n";
  } else {
    std::cout << label << ": FAIL\n";
    ++failures;
  }
}

}  // namespace

int main() {
  using group_control::ActionManager;
  using group_control::ConnectionState;
  using group_control::GroupController;
  using group_control::RobotConfig;
  using group_control::RobotManager;

  RobotConfig robot_a{"Robot_01", "192.168.1.120", 43893, 43897};
  RobotConfig robot_b{"Robot_02", "0.0.0.0", 43893, 43898};

  RobotManager robot_manager;
  Check(robot_manager.addRobot(robot_a), "Robot_01 create");
  Check(robot_manager.addRobot(robot_b), "Robot_02 create");
  Check(robot_manager.size() == 2U, "RobotManager size after add");
  Check(robot_manager.hasRobot("Robot_01"), "RobotManager has Robot_01");
  Check(robot_manager.hasRobot("Robot_02"), "RobotManager has Robot_02");

  auto* found_a = robot_manager.getRobot("Robot_01");
  auto* found_b = robot_manager.getRobot("Robot_02");
  Check(found_a != nullptr, "get Robot_01");
  Check(found_b != nullptr, "get Robot_02");
  Check(found_a != found_b, "Robot_01 != Robot_02");
  Check(found_a != nullptr && found_a->robotId() == "Robot_01", "Robot_01 identity");
  Check(found_b != nullptr && found_b->robotId() == "Robot_02", "Robot_02 identity");
  Check(found_a != nullptr && found_a->ip() == "192.168.1.120", "Robot_01 IP");
  Check(found_a != nullptr && found_a->targetPort() == 43893, "Robot_01 target port");
  Check(found_a != nullptr && found_a->localStatePort() == 43897,
        "Robot_01 local state port");

  Check(!robot_manager.addRobot(robot_a), "Duplicate Robot_01 rejected");
  Check(robot_manager.getAllRobots().size() == 2U, "get all robots");

  ActionManager action_manager = ActionManager::createDefaultDryRunManager();
  Check(action_manager.hasAction("stand"), "ActionManager stand registered");
  Check(action_manager.hasAction("squat"), "ActionManager squat registered");
  Check(action_manager.hasAction("squat_low"), "ActionManager squat_low registered");
  Check(action_manager.hasAction("squat_mid"), "ActionManager squat_mid registered");
  Check(action_manager.hasAction("squat_high"), "ActionManager squat_high registered");
  Check(action_manager.hasAction("fast_squat"),
        "ActionManager fast_squat registered");
  Check(action_manager.execute(robot_manager, "Robot_01", "squat"),
        "Squat Action dry-run interface");
  Check(found_a != nullptr &&
            found_a->connectionState() == ConnectionState::kMockConnected,
        "Robot_01 mock connected after dry-run");

  GroupController group_controller(robot_manager, action_manager);
  auto robot_a_records = group_controller.runOne("Robot_01", "squat_low");
  Check(robot_a_records.size() == 1U, "GroupController Robot_01 one action count");
  Check(robot_a_records[0].robot_id == "Robot_01", "GroupController Robot_01 isolated");
  Check(robot_a_records[0].action == "squat_low", "GroupController Robot_01 action");

  auto robot_b_records = group_controller.runOne("Robot_02", "squat_high");
  Check(robot_b_records.size() == 1U, "GroupController Robot_02 one action count");
  Check(robot_b_records[0].robot_id == "Robot_02", "GroupController Robot_02 isolated");
  Check(robot_b_records[0].action == "squat_high", "GroupController Robot_02 action");

  auto sync_records = group_controller.runGroup({"Robot_01", "Robot_02"}, "squat");
  Check(sync_records.size() == 2U, "GroupController sync count");
  Check(sync_records[0].robot_id == "Robot_01", "Sync first robot 01");
  Check(sync_records[1].robot_id == "Robot_02", "Sync second robot 02");
  Check(sync_records[0].action == "squat" && sync_records[1].action == "squat",
        "Sync same action");

  auto different_records = group_controller.runDifferentActions({
      {"Robot_01", "squat_low", 0.0},
      {"Robot_02", "squat_high", 0.0},
  });
  Check(different_records.size() == 2U, "Different actions count");
  Check(different_records[0].robot_id == "Robot_01" &&
            different_records[0].action == "squat_low",
        "Different action Robot_01 low");
  Check(different_records[1].robot_id == "Robot_02" &&
            different_records[1].action == "squat_high",
        "Different action Robot_02 high");

  auto swapped_records = group_controller.runDifferentActions({
      {"Robot_01", "squat_high", 0.0},
      {"Robot_02", "squat_low", 0.0},
  });
  Check(swapped_records[0].robot_id == "Robot_01" &&
            swapped_records[0].action == "squat_high",
        "Swapped Robot_01 high");
  Check(swapped_records[1].robot_id == "Robot_02" &&
            swapped_records[1].action == "squat_low",
        "Swapped Robot_02 low");

  auto delayed_records = group_controller.runDifferentActions({
      {"Robot_01", "squat_low", 0.0},
      {"Robot_02", "squat_low", 0.15},
  });
  Check(delayed_records.size() == 2U, "Delayed actions count");
  Check(std::fabs(delayed_records[1].planned_start - 0.15) < 0.001,
        "Delayed planned start recorded");
  Check(delayed_records[1].actual_dispatch >= 0.12,
        "Delayed actual dispatch recorded");

  auto sequence_records = group_controller.runSequence({
      {"Robot_01", "squat_low", 0.00},
      {"Robot_02", "squat_high", 0.05},
      {"Robot_01", "squat_high", 0.10},
      {"Robot_02", "squat_low", 0.15},
  });
  Check(sequence_records.size() == 4U, "Sequence count");
  Check(sequence_records[0].robot_id == "Robot_01" &&
            sequence_records[0].action == "squat_low",
        "Sequence A low");
  Check(sequence_records[1].robot_id == "Robot_02" &&
            sequence_records[1].action == "squat_high",
        "Sequence B high");
  Check(sequence_records[2].robot_id == "Robot_01" &&
            sequence_records[2].action == "squat_high",
        "Sequence A high");
  Check(sequence_records[3].robot_id == "Robot_02" &&
            sequence_records[3].action == "squat_low",
        "Sequence B low");

  Check(robot_manager.removeRobot("Robot_02"), "Remove Robot_02");
  Check(!robot_manager.hasRobot("Robot_02"), "Robot_02 removed");
  Check(robot_manager.size() == 1U, "RobotManager size after remove");

  if (failures == 0) {
    std::cout << "All group_control tests passed.\n";
  }
  return failures == 0 ? 0 : 1;
}
