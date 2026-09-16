# 环境配置

## 基础要求

- 安装 ROS2 的 Ubuntu 开发环境。
- `colcon` 和支持 C++17 的编译器。
- 单独准备 DeepRobotics Lite3 MotionSDK。
- 如果要运行单机器人动作，需要同一工作空间中存在配套 `transfer` 包。

官方 SDK 不包含在本仓库中，应该放在仓库外部。

## 构建

```bash
mkdir -p ~/lite3_group_ws/src
cd ~/lite3_group_ws/src
git clone https://github.com/jisoolili/lite3-group-control.git
cd ~/lite3_group_ws
colcon build --packages-select group_control
source install/setup.bash
```

## 测试

```bash
colcon test --packages-select group_control
colcon test-result --verbose
```

也可以在构建后直接运行测试程序：

```bash
ros2 run group_control test_robot_manager
```

## 空跑演示

```bash
ros2 run group_control dual_robot_demo --dry-run
ros2 run group_control dual_robot_demo --sequence --dry-run
```

当前 `dual_robot_demo` 会阻止真实执行，必须显式传入 `--dry-run`。
