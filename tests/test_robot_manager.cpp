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

  RobotConfig robot_a{"Robot_A", "192.168.1.120", 43893, 43897,
                      "192.168.123.99", "enx00e04c36bbfd", 43897,
                      "udp", "wired", "transfer/lite3_stand_demo", true};
  RobotConfig robot_b{"Robot_B", "192.168.1.120", 43893, 43897,
                      "192.168.1.99", "enp2s0", 43897,
                      "udp", "wired", "transfer/lite3_stand_demo", true};

  RobotManager robot_manager;
  Check(robot_manager.addRobot(robot_a), "Robot_A create");
  Check(robot_manager.addRobot(robot_b), "Robot_B create");
  Check(robot_manager.size() == 2U, "RobotManager size after add");
  Check(robot_manager.hasRobot("Robot_A"), "RobotManager has Robot_A");
  Check(robot_manager.hasRobot("Robot_B"), "RobotManager has Robot_B");

  auto* found_a = robot_manager.getRobot("Robot_A");
  auto* found_b = robot_manager.getRobot("Robot_B");
  Check(found_a != nullptr, "get Robot_A");
  Check(found_b != nullptr, "get Robot_B");
  Check(found_a != found_b, "Robot_A != Robot_B");
  Check(found_a != nullptr && found_a->robotId() == "Robot_A", "Robot_A identity");
  Check(found_b != nullptr && found_b->robotId() == "Robot_B", "Robot_B identity");
  Check(found_a != nullptr && found_a->ip() == "192.168.1.120", "Robot_A IP");
  Check(found_a != nullptr && found_a->targetPort() == 43893, "Robot_A target port");
  Check(found_a != nullptr && found_a->localStatePort() == 43897,
        "Robot_A local state port");
  Check(found_a != nullptr && found_a->localIp() == "192.168.123.99",
        "Robot_A local IP");
  Check(found_a != nullptr && found_a->bindInterface() == "enx00e04c36bbfd",
        "Robot_A bind interface");
  Check(found_a != nullptr && found_a->statePort() == 43897,
        "Robot_A state port");
  Check(found_b != nullptr && found_b->localIp() == "192.168.1.99",
        "Robot_B local IP");
  Check(found_b != nullptr && found_b->bindInterface() == "enp2s0",
        "Robot_B bind interface");
  Check(found_b != nullptr && found_b->statePort() == 43897,
        "Robot_B state port");
  Check(found_a != nullptr && found_a->transport() == "udp", "Robot_A transport");
  Check(found_a != nullptr && found_a->network() == "wired", "Robot_A network");
  Check(found_a != nullptr && found_a->actionInterface() == "transfer/lite3_stand_demo",
        "Robot_A action interface");
  Check(found_a != nullptr && found_a->actionVerified(), "Robot_A action verified flag");
  Check(found_b != nullptr && found_b->actionVerified(),
        "Robot_B action verified flag");

  Check(!robot_manager.addRobot(robot_a), "Duplicate Robot_A rejected");
  Check(robot_manager.getAllRobots().size() == 2U, "get all robots");

  ActionManager action_manager = ActionManager::createDefaultDryRunManager();
  Check(action_manager.hasAction("stand"), "ActionManager stand registered");
  Check(action_manager.hasAction("squat"), "ActionManager squat registered");
  Check(action_manager.hasAction("squat_low"), "ActionManager squat_low registered");
  Check(action_manager.hasAction("squat_mid"), "ActionManager squat_mid registered");
  Check(action_manager.hasAction("squat_high"), "ActionManager squat_high registered");
  Check(action_manager.hasAction("fast_squat"),
        "ActionManager fast_squat registered");
  Check(action_manager.hasAction("sway"), "ActionManager sway registered");
  Check(action_manager.hasAction("pitch"), "ActionManager pitch registered");
  Check(action_manager.hasAction("leg_lift"), "ActionManager leg_lift registered");
  Check(action_manager.hasAction("rotate"), "ActionManager rotate registered");
  Check(action_manager.execute(robot_manager, "Robot_A", "squat"),
        "Squat Action dry-run interface");
  Check(found_a != nullptr &&
            found_a->connectionState() == ConnectionState::kMockConnected,
        "Robot_A mock connected after dry-run");

  GroupController group_controller(robot_manager, action_manager);
  auto robot_a_records = group_controller.runOne("Robot_A", "squat_low");
  Check(robot_a_records.size() == 1U, "GroupController Robot_A one action count");
  Check(robot_a_records[0].robot_id == "Robot_A", "GroupController Robot_A isolated");
  Check(robot_a_records[0].action == "squat_low", "GroupController Robot_A action");

  auto robot_b_records = group_controller.runOne("Robot_B", "squat_high");
  Check(robot_b_records.size() == 1U, "GroupController Robot_B one action count");
  Check(robot_b_records[0].robot_id == "Robot_B", "GroupController Robot_B isolated");
  Check(robot_b_records[0].action == "squat_high", "GroupController Robot_B action");

  auto sync_records = group_controller.runGroup({"Robot_A", "Robot_B"}, "squat");
  Check(sync_records.size() == 2U, "GroupController sync count");
  Check(sync_records[0].robot_id == "Robot_A", "Sync first robot A");
  Check(sync_records[1].robot_id == "Robot_B", "Sync second robot B");
  Check(sync_records[0].action == "squat" && sync_records[1].action == "squat",
        "Sync same action");

  auto different_records = group_controller.runDifferentActions({
      {"Robot_A", "squat_low", 0.0},
      {"Robot_B", "squat_high", 0.0},
  });
  Check(different_records.size() == 2U, "Different actions count");
  Check(different_records[0].robot_id == "Robot_A" &&
            different_records[0].action == "squat_low",
        "Different action Robot_A low");
  Check(different_records[1].robot_id == "Robot_B" &&
            different_records[1].action == "squat_high",
        "Different action Robot_B high");

  auto swapped_records = group_controller.runDifferentActions({
      {"Robot_A", "squat_high", 0.0},
      {"Robot_B", "squat_low", 0.0},
  });
  Check(swapped_records[0].robot_id == "Robot_A" &&
            swapped_records[0].action == "squat_high",
        "Swapped Robot_A high");
  Check(swapped_records[1].robot_id == "Robot_B" &&
            swapped_records[1].action == "squat_low",
        "Swapped Robot_B low");

  auto delayed_records = group_controller.runDifferentActions({
      {"Robot_A", "squat_low", 0.0},
      {"Robot_B", "squat_low", 0.15},
  });
  Check(delayed_records.size() == 2U, "Delayed actions count");
  Check(std::fabs(delayed_records[1].planned_start - 0.15) < 0.001,
        "Delayed planned start recorded");
  Check(delayed_records[1].actual_dispatch >= 0.12,
        "Delayed actual dispatch recorded");

  auto sequence_records = group_controller.runSequence({
      {"Robot_A", "squat_low", 0.00},
      {"Robot_B", "squat_high", 0.05},
      {"Robot_A", "squat_high", 0.10},
      {"Robot_B", "squat_low", 0.15},
  });
  Check(sequence_records.size() == 4U, "Sequence count");
  Check(sequence_records[0].robot_id == "Robot_A" &&
            sequence_records[0].action == "squat_low",
        "Sequence A low");
  Check(sequence_records[1].robot_id == "Robot_B" &&
            sequence_records[1].action == "squat_high",
        "Sequence B high");
  Check(sequence_records[2].robot_id == "Robot_A" &&
            sequence_records[2].action == "squat_high",
        "Sequence A high");
  Check(sequence_records[3].robot_id == "Robot_B" &&
            sequence_records[3].action == "squat_low",
        "Sequence B low");

  Check(robot_manager.removeRobot("Robot_B"), "Remove Robot_B");
  Check(!robot_manager.hasRobot("Robot_B"), "Robot_B removed");
  Check(robot_manager.size() == 1U, "RobotManager size after remove");

  if (failures == 0) {
    std::cout << "All group_control tests passed.\n";
  }
  return failures == 0 ? 0 : 1;
}
