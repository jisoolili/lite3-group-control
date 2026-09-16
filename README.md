# lite3-group-control

DeepRobotics Lite3 multi-robot group control and stage action coordination system.

The project currently provides a ROS2 `ament_cmake` package for dry-run validation of robot identity, action dispatch, dual-robot scheduling, delayed execution, and sequence execution. It is intended to grow from 2 Lite3 robots to 5 and then 10 robots for stage performance and music-show control.

## Current Capability

Implemented:

- `RobotManager` for robot registration and lookup.
- `ActionManager` for named action registration and dispatch.
- `GroupController` for single robot, synchronized group, delayed, different-action, and sequence dispatch.
- `dual_robot_demo` dry-run executable.
- Unit-style executable test for robot/action/group abstractions.

Validated:

- RobotState over wired network for Robot A baseline.
- Single-robot `stand`, `squat-1-3`, `squat-2-3`, `squat-full` dry-runs in the companion `transfer` package.
- Current group control package in dry-run only.

Planned:

- Real multi-robot transport integration.
- `SequenceManager` as a first-class timeline layer.
- Parameterized `Wave` action for 2 to 10 robots.
- Additional stage actions: `Height`, `FastSquat`, `Sway`, `Pitch`, `LegLift`, `Rotate`.

## Architecture

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

Robot IDs are logical names such as `Robot_01`, `Robot_02`, and `Robot_10`. A Robot ID maps to configuration, and configuration maps to IP address and ports. Control logic should not depend on a specific IP address.

## Repository Layout

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

## Build

Clone this repository into a ROS2 workspace `src` directory, then build with `colcon`:

```bash
mkdir -p ~/lite3_group_ws/src
cd ~/lite3_group_ws/src
git clone https://github.com/jisoolili/lite3-group-control.git
cd ~/lite3_group_ws
colcon build --packages-select group_control
source install/setup.bash
```

## Dry-Run Demo

The current demo is intentionally dry-run only:

```bash
ros2 run group_control dual_robot_demo --dry-run
ros2 run group_control dual_robot_demo --robot Robot_01 --action squat_low --dry-run
ros2 run group_control dual_robot_demo --robot-a-action squat_low --robot-b-action squat_high --dry-run
ros2 run group_control dual_robot_demo --robot-a-action squat_low --robot-b-action squat_low --delay 0.25 --dry-run
ros2 run group_control dual_robot_demo --sequence --dry-run
```

Real robot commands are documented away from the README front page. Read [docs/safety.md](docs/safety.md) first, then use [docs/single-robot-actions.md](docs/single-robot-actions.md).

## SDK Requirement

This repository does not vendor the DeepRobotics Lite3 MotionSDK. Developers must prepare the official SDK separately, for example:

```text
/home/charlie/Lite3_MotionSDK-main
```

Do not commit official SDK source, vendor binaries, robot internal files, private keys, passwords, or tokens to this repository.

## Documentation

- [Architecture](docs/architecture.md)
- [Setup](docs/setup.md)
- [Robot network](docs/robot-network.md)
- [Single robot actions](docs/single-robot-actions.md)
- [Dual robot control](docs/dual-robot-control.md)
- [Wave demo](docs/wave-demo.md)
- [Scaling to 10 robots](docs/scaling-to-10-robots.md)
- [Safety](docs/safety.md)
