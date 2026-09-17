#include <cstdlib>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "group_control/group_controller.hpp"

namespace {

struct Options {
  bool dry_run = false;
  bool real_run_confirm = false;
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
      << executable << " --robot Robot_A --action squat_low --dry-run\n"
      << executable << " --robot Robot_A --action stand --real-run-confirm\n"
      << executable << " --action squat --dry-run\n"
      << executable
      << " --robot-a-action squat_low --robot-b-action squat_high --dry-run\n"
      << executable
      << " --robot-a-action squat_low --robot-b-action squat_low --delay 0.25 --dry-run\n"
      << executable
      << " --robot-a-action stand --robot-b-action stand --delay 0.25 --real-run-confirm\n"
      << executable << " --sequence --dry-run\n";
}

std::string ShellQuote(const std::string& value) {
  std::string quoted = "'";
  for (const char c : value) {
    if (c == '\'') {
      quoted += "'\\''";
    } else {
      quoted += c;
    }
  }
  quoted += "'";
  return quoted;
}

long long NowNs() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

double ElapsedMs(const long long start_ns) {
  return static_cast<double>(NowNs() - start_ns) / 1000000.0;
}

std::string DirName(const std::string& path) {
  const std::string::size_type slash = path.find_last_of('/');
  return slash == std::string::npos ? "." : path.substr(0, slash);
}

bool IsExecutableFile(const std::string& path) {
  return !path.empty() && access(path.c_str(), X_OK) == 0;
}

std::string Lite3ExecutableUnderWorkspace(const std::string& workspace_root) {
  return workspace_root +
         "/install/transfer/lib/transfer/lite3_stand_demo";
}

std::string FindLite3ExecutableFrom(const std::string& start_dir) {
  std::string dir = start_dir;
  for (int depth = 0; depth < 10; ++depth) {
    const std::string candidate = Lite3ExecutableUnderWorkspace(dir);
    if (IsExecutableFile(candidate)) {
      return candidate;
    }

    const std::string parent = DirName(dir);
    if (parent == dir || parent.empty()) {
      break;
    }
    dir = parent;
  }
  return "";
}

std::string Lite3ExecutablePath() {
  const char* override_path = std::getenv("LITE3_STAND_DEMO_PATH");
  if (override_path != nullptr && override_path[0] != '\0') {
    return override_path;
  }

  char path[4096];
  const ssize_t size = readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (size <= 0) {
    return "lite3_stand_demo";
  }
  path[size] = '\0';

  const std::string executable_path(path);
  const std::string install_marker = "/install/";
  const std::string::size_type install_pos =
      executable_path.find(install_marker);
  if (install_pos != std::string::npos) {
    const std::string workspace_root = executable_path.substr(0, install_pos);
    const std::string candidate = Lite3ExecutableUnderWorkspace(workspace_root);
    if (IsExecutableFile(candidate)) {
      return candidate;
    }
  }

  const std::string found = FindLite3ExecutableFrom(DirName(executable_path));
  if (!found.empty()) {
    return found;
  }

  const std::string default_workspace_candidate =
      Lite3ExecutableUnderWorkspace("/home/charlie/lite3_ros_ws");
  if (IsExecutableFile(default_workspace_candidate)) {
    return default_workspace_candidate;
  }

  return "lite3_stand_demo";
}

bool Lite3ActionFor(const std::string& action, std::string* lite3_action) {
  static const std::map<std::string, std::string> kActions = {
      {"stand", "stand"},
      {"squat_low", "squat-1-3"},
      {"squat_mid", "squat-2-3"},
      {"squat_high", "squat-full"},
      {"fast_squat", "fast-squat"},
  };
  const auto it = kActions.find(action);
  if (it == kActions.end()) {
    return false;
  }
  *lite3_action = it->second;
  return true;
}

std::string BuildRealCommand(const group_control::Robot& robot,
                             const std::string& action) {
  std::string lite3_action;
  if (!Lite3ActionFor(action, &lite3_action)) {
    return "";
  }

  std::ostringstream command;
  command << "ros2 run transfer lite3_stand_demo"
          << " --real-run-confirm"
          << " --action " << ShellQuote(lite3_action)
          << " --robot-ip " << ShellQuote(robot.ip())
          << " --robot-port " << robot.targetPort()
          << " --local-ip " << ShellQuote(robot.localIp())
          << " --bind-interface " << ShellQuote(robot.bindInterface())
          << " --state-port " << robot.statePort();
  return command.str();
}

std::vector<std::string> BuildRealArgs(const std::string& executable,
                                       const group_control::Robot& robot,
                                       const std::string& action) {
  std::string lite3_action;
  if (!Lite3ActionFor(action, &lite3_action)) {
    return {};
  }
  return {
      executable,
      "--real-run-confirm",
      "--action",
      lite3_action,
      "--robot-ip",
      robot.ip(),
      "--robot-port",
      std::to_string(robot.targetPort()),
      "--local-ip",
      robot.localIp(),
      "--bind-interface",
      robot.bindInterface(),
      "--state-port",
      std::to_string(robot.statePort()),
  };
}

std::string JoinCommandForLog(const std::vector<std::string>& args) {
  std::ostringstream command;
  for (std::size_t i = 0; i < args.size(); ++i) {
    if (i != 0U) {
      command << ' ';
    }
    command << ShellQuote(args[i]);
  }
  return command.str();
}

int ExecLite3Process(const std::vector<std::string>& args,
                     const std::string& robot_id,
                     const long long t0_ns) {
  if (args.empty()) {
    return 127;
  }

  std::vector<char*> argv;
  argv.reserve(args.size() + 1U);
  for (const auto& arg : args) {
    argv.push_back(const_cast<char*>(arg.c_str()));
  }
  argv.push_back(nullptr);

  const pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "[TIMING] robot=" << robot_id
              << " event=T1 fork_failed elapsed_ms=" << ElapsedMs(t0_ns)
              << " errno=" << errno << " error=" << std::strerror(errno)
              << "\n";
    return 127;
  }

  if (pid == 0) {
    const std::string t0 = std::to_string(t0_ns);
    setenv("LITE3_GROUP_T0_NS", t0.c_str(), 1);
    setenv("LITE3_GROUP_ROBOT_ID", robot_id.c_str(), 1);
    execv(args[0].c_str(), argv.data());
    std::cerr << "[TIMING] robot=" << robot_id
              << " event=exec_failed errno=" << errno
              << " error=" << std::strerror(errno) << "\n";
    _exit(127);
  }

  std::cout << "[TIMING] robot=" << robot_id << " event=T1_CHILD_CREATED"
            << " pid=" << pid << " elapsed_ms=" << ElapsedMs(t0_ns) << "\n";

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    std::cerr << "[TIMING] robot=" << robot_id
              << " event=waitpid_failed elapsed_ms=" << ElapsedMs(t0_ns)
              << " errno=" << errno << " error=" << std::strerror(errno)
              << "\n";
    return 127;
  }
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    return 128 + WTERMSIG(status);
  }
  return 127;
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
    } else if (arg == "--real-run-confirm") {
      options->real_run_confirm = true;
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
  if (options->dry_run == options->real_run_confirm) {
    std::cerr << "ERROR: pass exactly one of --dry-run or --real-run-confirm.\n";
    return false;
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  using group_control::ActionCommand;
  using group_control::ActionManager;
  using group_control::GroupController;
  using group_control::Robot;
  using group_control::RobotConfig;
  using group_control::RobotManager;

  Options options;
  const long long t0_ns = NowNs();
  if (!ParseArgs(argc, argv, &options)) {
    PrintUsage(argv[0]);
    return 2;
  }

  RobotManager robot_manager;
  robot_manager.addRobot(RobotConfig{"Robot_A", "192.168.1.120", 43893, 43897,
                                      "192.168.123.99", "enx00e04c36bbfd", 43897,
                                      "udp", "wired", "transfer/lite3_stand_demo",
                                      true});
  robot_manager.addRobot(RobotConfig{"Robot_B", "192.168.1.120", 43893, 43897,
                                      "192.168.1.99", "enp2s0", 43897,
                                      "udp", "wired", "transfer/lite3_stand_demo",
                                      true});

  ActionManager action_manager = ActionManager::createDefaultDryRunManager();
  GroupController controller(robot_manager, action_manager);

  std::cout << "[GROUP CONTROL]\n"
            << "mode=" << (options.dry_run ? "DRY_RUN" : "REAL_RUN_CONFIRM")
            << "\n";
  std::cout << "[TIMING] event=T0_DUAL_COMMAND_START elapsed_ms=0.000\n";

  if (options.sequence) {
    std::vector<ActionCommand> commands = {
        ActionCommand{"Robot_A", "squat_low", 0.00},
        ActionCommand{"Robot_B", "squat_low", 0.25},
        ActionCommand{"Robot_A", "squat_high", 0.50},
        ActionCommand{"Robot_B", "squat_high", 0.75},
    };
    if (options.dry_run) {
      std::cout << "NO COMMAND SENT TO ROBOT\n";
      controller.runSequence(commands);
    } else {
      std::cerr << "REAL RUN BLOCKED: --sequence is dry-run only for now.\n";
      return 3;
    }
    return 0;
  }

  if (!options.robot_id.empty()) {
    if (options.dry_run) {
      std::cout << "NO COMMAND SENT TO ROBOT\n";
      controller.runOne(options.robot_id, options.action);
    } else {
      Robot* robot = robot_manager.getRobot(options.robot_id);
      if (robot == nullptr) {
        std::cerr << "ERROR: unknown robot: " << options.robot_id << "\n";
        return 3;
      }
      const std::string command = BuildRealCommand(*robot, options.action);
      const std::vector<std::string> args =
          BuildRealArgs(Lite3ExecutablePath(), *robot, options.action);
      if (command.empty() || args.empty()) {
        std::cerr << "REAL RUN BLOCKED: action is not approved for dual real run: "
                  << options.action << "\n";
        return 4;
      }
      std::cout << "[REAL RUN COMMAND]\n"
                << "robot=" << robot->robotId() << "\n"
                << "ip=" << robot->ip() << "\n"
                << "port=" << robot->targetPort() << "\n"
                << "interface=" << robot->bindInterface() << "\n"
                << "bind_interface=" << robot->bindInterface() << "\n"
                << "local_ip=" << robot->localIp() << "\n"
                << "state_port=" << robot->statePort() << "\n"
                << "command=" << JoinCommandForLog(args) << "\n";
      return ExecLite3Process(args, robot->robotId(), t0_ns);
    }
    return 0;
  }

  if (!options.robot_a_action.empty() || !options.robot_b_action.empty()) {
    const std::string action_a =
        options.robot_a_action.empty() ? options.action : options.robot_a_action;
    const std::string action_b =
        options.robot_b_action.empty() ? options.action : options.robot_b_action;
    std::vector<ActionCommand> commands = {
        ActionCommand{"Robot_A", action_a, 0.0},
        ActionCommand{"Robot_B", action_b, options.delay},
    };
    if (options.dry_run) {
      std::cout << "NO COMMAND SENT TO ROBOT\n";
      controller.runDifferentActions(commands);
    } else {
      struct RealRunPlan {
        const Robot* robot = nullptr;
        ActionCommand command;
        std::vector<std::string> args;
      };
      std::vector<RealRunPlan> real_run_plan;
      const std::string lite3_executable = Lite3ExecutablePath();
      for (const auto& planned : commands) {
        Robot* robot = robot_manager.getRobot(planned.robot_id);
        const std::vector<std::string> args =
            robot == nullptr ? std::vector<std::string>{}
                             : BuildRealArgs(lite3_executable, *robot, planned.action);
        if (robot == nullptr || args.empty()) {
          std::cerr << "REAL RUN BLOCKED: invalid robot/action "
                    << planned.robot_id << " " << planned.action << "\n";
          return 4;
        }
        real_run_plan.push_back(RealRunPlan{robot, planned, args});
      }

      std::vector<std::thread> threads;
      std::vector<int> exit_codes(real_run_plan.size(), 0);
      for (const auto& planned : real_run_plan) {
        const std::size_t index = threads.size();
        std::cout << "[REAL RUN COMMAND]\n"
                  << "robot=" << planned.robot->robotId() << "\n"
                  << "action=" << planned.command.action << "\n"
                  << "delay=" << planned.command.start_delay << "\n"
                  << "ip=" << planned.robot->ip() << "\n"
                  << "port=" << planned.robot->targetPort() << "\n"
                  << "interface=" << planned.robot->bindInterface() << "\n"
                  << "bind_interface=" << planned.robot->bindInterface() << "\n"
                  << "local_ip=" << planned.robot->localIp() << "\n"
                  << "state_port=" << planned.robot->statePort() << "\n"
                  << "command=" << JoinCommandForLog(planned.args) << "\n";
        threads.emplace_back([planned, &exit_codes, index, t0_ns]() {
          std::this_thread::sleep_for(
              std::chrono::duration<double>(planned.command.start_delay));
          exit_codes[index] = ExecLite3Process(
              planned.args, planned.robot->robotId(), t0_ns);
        });
      }
      for (auto& thread : threads) {
        thread.join();
      }
      for (const int code : exit_codes) {
        if (code != 0) {
          return code;
        }
      }
      return 0;
    }
    return 0;
  }

  if (options.dry_run) {
    std::cout << "NO COMMAND SENT TO ROBOT\n";
    controller.runGroup({"Robot_A", "Robot_B"}, options.action);
  } else {
    std::cerr << "REAL RUN BLOCKED: use --robot-a-action/--robot-b-action for"
              << " explicit dual real run.\n";
    return 3;
  }
  return 0;
}
