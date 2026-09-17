# 双机器人控制

当前包支持两个逻辑机器人的 dry-run 验证。

## Robot ID

统一使用逻辑 ID：

```text
Robot_A
Robot_B
```

Robot ID 不是 IP 地址。IP 地址只属于机器人配置。

## 空跑示例

两台机器人执行同一动作：

```bash
ros2 run group_control dual_robot_demo --action squat --dry-run
```

单机器人：

```bash
ros2 run group_control dual_robot_demo --robot Robot_A --action squat_low --dry-run
```

不同动作：

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action squat_low \
  --robot-b-action squat_high \
  --dry-run
```

延迟执行：

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action squat_low \
  --robot-b-action squat_low \
  --delay 0.25 \
  --dry-run
```

连续序列：

```bash
ros2 run group_control dual_robot_demo --sequence --dry-run
```

新表演动作 dry-run：

```bash
ros2 run group_control dual_robot_demo --robot Robot_A --action sway --dry-run
ros2 run group_control dual_robot_demo --robot Robot_A --action pitch --dry-run
ros2 run group_control dual_robot_demo --robot Robot_A --action leg_lift --dry-run
ros2 run group_control dual_robot_demo --robot Robot_A --action rotate --dry-run
```

注意：`sway`、`pitch`、`leg_lift`、`rotate` 当前只完成 dry-run 接入，不能写成真实机器人已验证。

## API 方向

未来高层控制接口应接近：

```cpp
group.execute(
    robots = {"Robot_A", "Robot_B"},
    action = "squat");
```

定时序列应接近：

```cpp
sequence = {
    {"Robot_A", "wave", 0.00},
    {"Robot_B", "wave", 0.25},
};
```

当前 demo 尚未开放真实双机器人执行。
