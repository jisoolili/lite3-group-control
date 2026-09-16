#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "group_control/group_controller.hpp"

namespace {

struct Options {
  bool dry_run = false;
  bool sequence = false;
  std::string robot_id;
  std::string action = "squat";
  std::string robot_a_action;
  std::string robot_b_action;
  double delay = 0.0;
};

void PrintUsage(const char* executable) {
  std::cout
      << "Usage:\n"
      << executable << " --robot Robot_01 --action squat_low --dry-run\n"
      << executable << " --action squat --dry-run\n"
      << executable
      << " --robot-a-action squat_low --robot-b-action squat_high --dry-run\n"
      << executable
      << " --robot-a-action squat_low --robot-b-action squat_low --delay 0.25 --dry-run\n"
      << executable << " --sequence --dry-run\n";
}

bool ParseDouble(const std::string& value, double* parsed) {
  try {
    *parsed = std::stod(value);
    return *parsed >= 0.0;
  } catch (...) {
    return false;
  }
}

bool ParseArgs(int argc, char** argv, Options* options) {
  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);
    if (arg == "--dry-run") {
      options->dry_run = true;
    } else if (arg == "--sequence") {
      options->sequence = true;
    } else if (arg == "--robot" && i + 1 < argc) {
      options->robot_id = argv[++i];
    } else if (arg == "--action" && i + 1 < argc) {
      options->action = argv[++i];
    } else if (arg == "--robot-a-action" && i + 1 < argc) {
      options->robot_a_action = argv[++i];
    } else if (arg == "--robot-b-action" && i + 1 < argc) {
      options->robot_b_action = argv[++i];
    } else if (arg == "--delay" && i + 1 < argc) {
      if (!ParseDouble(argv[++i], &options->delay)) {
        std::cerr << "ERROR: invalid --delay value\n";
        return false;
      }
    } else if (arg == "--help" || arg == "-h") {
      PrintUsage(argv[0]);
      std::exit(0);
    } else {
      std::cerr << "ERROR: unknown argument: " << arg << "\n";
      return false;
    }
  }
  if (!options->dry_run) {
    std::cerr << "REAL RUN BLOCKED: pass --dry-run for stage 3 validation.\n";
    return false;
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  using group_control::ActionCommand;
  using group_control::ActionManager;
  using group_control::GroupController;
  using group_control::RobotConfig;
  using group_control::RobotManager;

  Options options;
  if (!ParseArgs(argc, argv, &options)) {
    PrintUsage(argv[0]);
    return 2;
  }

  RobotManager robot_manager;
  robot_manager.addRobot(RobotConfig{"Robot_01", "192.168.1.120", 43893, 43897});
  robot_manager.addRobot(RobotConfig{"Robot_02", "0.0.0.0", 43893, 43898});

  ActionManager action_manager = ActionManager::createDefaultDryRunManager();
  GroupController controller(robot_manager, action_manager);

  std::cout << "[GROUP CONTROL]\n"
            << "mode=DRY_RUN\n"
            << "NO COMMAND SENT TO ROBOT\n";

  if (options.sequence) {
    controller.runSequence({
        ActionCommand{"Robot_01", "squat_low", 0.00},
        ActionCommand{"Robot_02", "squat_low", 0.25},
        ActionCommand{"Robot_01", "squat_high", 0.50},
        ActionCommand{"Robot_02", "squat_high", 0.75},
    });
    return 0;
  }

  if (!options.robot_id.empty()) {
    controller.runOne(options.robot_id, options.action);
    return 0;
  }

  if (!options.robot_a_action.empty() || !options.robot_b_action.empty()) {
    const std::string action_a =
        options.robot_a_action.empty() ? options.action : options.robot_a_action;
    const std::string action_b =
        options.robot_b_action.empty() ? options.action : options.robot_b_action;
    controller.runDifferentActions({
        ActionCommand{"Robot_01", action_a, 0.0},
        ActionCommand{"Robot_02", action_b, options.delay},
    });
    return 0;
  }

  controller.runGroup({"Robot_01", "Robot_02"}, options.action);
  return 0;
}
