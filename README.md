# lite3-group-control

DeepRobotics Lite3 多机器人群控与演出动作控制系统。

当前仓库提供一个 ROS2 `ament_cmake` 包，用于验证机器人 ID 管理、动作分发、双机器人 dry-run 调度、延迟执行和连续动作序列。项目目标是从 2 台 Lite3 验证逐步扩展到 5 台、10 台，用于机器人舞台演出和音乐现场群控。

## 当前能力

已实现：

- `RobotManager`：机器人注册、查询和移除。
- `ActionManager`：动作注册、查询和分发。
- `GroupController`：单机器人、同步群组、延迟、不同动作、连续序列调度。
- `dual_robot_demo`：双机器人 dry-run 演示程序。
- `test_robot_manager`：机器人、动作、群控抽象的基础测试程序。

已验证：

- Robot A 有线 RobotState 链路基准。
- 配套 `transfer` 包中的单机器人 `stand`、`squat-1-3`、`squat-2-3`、`squat-full` dry-run。
- 当前 `group_control` 包仅完成 dry-run 级别验证。

规划中：

- 真实多机器人 Transport 接入。
- 独立 `SequenceManager` 时间线层。
- 支持 2 到 10 台机器人的参数化 `Wave` 声浪动作。
- 舞台动作库：`Height`、`FastSquat`、`Sway`、`Pitch`、`LegLift`、`Rotate`。

## 系统架构

```text
Show / Timeline
      |
      v
SequenceManager
      |
      v
GroupController
      |
      v
ActionManager
      |
      v
RobotManager
      |
      v
Robot_01 ... Robot_10
      |
      v
Transport
      |
      v
Lite3
```

Robot ID 使用 `Robot_01`、`Robot_02`、`Robot_10` 这类逻辑名称。Robot ID 映射到机器人配置，机器人配置再映射到 IP 和端口。控制逻辑不应该依赖某个固定 IP。

## 仓库结构

```text
.
|-- CMakeLists.txt
|-- package.xml
|-- config/
|-- docs/
|-- include/group_control/
|-- src/
|-- tests/
`-- scripts/
```

## 构建

把仓库克隆到 ROS2 workspace 的 `src` 目录后构建：

```bash
mkdir -p ~/lite3_group_ws/src
cd ~/lite3_group_ws/src
git clone https://github.com/jisoolili/lite3-group-control.git
cd ~/lite3_group_ws
colcon build --packages-select group_control
source install/setup.bash
```

## 空跑演示

当前演示程序只允许 dry-run：

```bash
ros2 run group_control dual_robot_demo --dry-run
ros2 run group_control dual_robot_demo --robot Robot_01 --action squat_low --dry-run
ros2 run group_control dual_robot_demo --robot-a-action squat_low --robot-b-action squat_high --dry-run
ros2 run group_control dual_robot_demo --robot-a-action squat_low --robot-b-action squat_low --delay 0.25 --dry-run
ros2 run group_control dual_robot_demo --sequence --dry-run
```

真实机器人命令不放在 README 顶部。执行真实动作前先阅读 [docs/safety.md](docs/safety.md)，再参考 [docs/single-robot-actions.md](docs/single-robot-actions.md)。

## SDK 要求

本仓库不包含 DeepRobotics Lite3 MotionSDK。开发者需要单独准备官方 SDK，例如：

```text
/home/charlie/Lite3_MotionSDK-main
```

不要把官方 SDK 源码、厂商二进制、机器人内部文件、私钥、密码或 token 提交到本仓库。

## 文档

- [系统架构](docs/architecture.md)
- [环境配置](docs/setup.md)
- [机器人网络](docs/robot-network.md)
- [单机器人动作](docs/single-robot-actions.md)
- [双机器人控制](docs/dual-robot-control.md)
- [Wave 声浪演示](docs/wave-demo.md)
- [扩展到 10 台机器人](docs/scaling-to-10-robots.md)
- [安全说明](docs/safety.md)
