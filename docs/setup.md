# Setup

## Requirements

- Ubuntu development machine with ROS2.
- `colcon` and a C++17 compiler.
- DeepRobotics Lite3 MotionSDK prepared separately.
- A Lite3 ROS workspace that contains the companion `transfer` package when running single-robot action demos.

The official SDK is not included in this repository. Keep the SDK outside the Git repository.

## Build

```bash
mkdir -p ~/lite3_group_ws/src
cd ~/lite3_group_ws/src
git clone https://github.com/jisoolili/lite3-group-control.git
cd ~/lite3_group_ws
colcon build --packages-select group_control
source install/setup.bash
```

## Test

```bash
colcon test --packages-select group_control
colcon test-result --verbose
```

Or run the executable test directly after build:

```bash
ros2 run group_control test_robot_manager
```

## Dry-Run Demo

```bash
ros2 run group_control dual_robot_demo --dry-run
ros2 run group_control dual_robot_demo --sequence --dry-run
```

The current `dual_robot_demo` blocks real runs and requires `--dry-run`.
