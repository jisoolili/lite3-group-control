# 双机器人控制

当前包支持两个逻辑机器人的 dry-run 验证，并提供 Ubuntu 开发电脑上的双狗真实群控入口。

目标是只在 Ubuntu 开发电脑执行一条 `ros2 run group_control dual_robot_demo ...` 命令，不需要 SSH 到 Robot_A 或 Robot_B 分别执行动作。

## Robot ID

统一使用逻辑 ID：

```text
Robot_A
Robot_B
```

Robot ID 不是 IP 地址。IP 地址只属于机器人配置。

## 固定网络映射

两台运动主机当前使用相同目标 IP `192.168.1.120`，必须通过 Ubuntu 的不同物理网口区分：

```text
Robot_A
ip=192.168.1.120
port=43893
local_ip=192.168.123.99
interface=enx00e04c36bbfd
state_port=43897

Robot_B
ip=192.168.1.120
port=43893
local_ip=192.168.1.99
interface=enp2s0
state_port=43897
```

`dual_robot_demo` 会把每台机器人的 `local_ip`、`bind_interface` 和 `state_port` 传给 `transfer/lite3_stand_demo`。`lite3_stand_demo` 再通过 UDP socket 绑定层让 SDK 的发送 socket 绑定到指定本地 IP 和指定物理网口。

因此 Robot_A 的控制 UDP 必须从 `enx00e04c36bbfd / 192.168.123.99` 发出，Robot_B 的控制 UDP 必须从 `enp2s0 / 192.168.1.99` 发出。

## RobotState

RobotState 当前按本地 IP 和端口区分：

```text
Robot_A -> 192.168.123.99:43897
Robot_B -> 192.168.1.99:43897
```

首次双狗真机测试前需要人工确认 Robot_A 和 Robot_B 运动主机上的 `/home/ysc/jy_exe/conf/network.toml` 中 `target_port` 与程序监听端口一致。当前默认配置要求两台机器人都回传到 `43897`，但本地监听 IP 不同，因此不会发生本地端口冲突。

本仓库不会自动修改机器人上的 `network.toml`，也不会自动重启 `jy_exe`。

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

## 真实双狗入口

真实执行必须显式传入 `--real-run-confirm`。当前真实执行白名单仅包含：

```text
stand
squat_low
squat_mid
squat_high
fast_squat
```

内部映射关系：

```text
squat_low  -> squat-1-3
squat_mid  -> squat-2-3
squat_high -> squat-full
fast_squat -> fast-squat
```

双狗同时 Stand：

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action stand \
  --robot-b-action stand \
  --delay 0 \
  --real-run-confirm
```

A/B 同时 Fast Squat：

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action fast_squat \
  --robot-b-action fast_squat \
  --delay 0 \
  --real-run-confirm
```

A 先动作，B 延迟 0.25 秒：

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action fast_squat \
  --robot-b-action fast_squat \
  --delay 0.25 \
  --real-run-confirm
```

A/B 不同动作：

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action squat_low \
  --robot-b-action squat_high \
  --delay 0.25 \
  --real-run-confirm
```

`sway`、`pitch`、`leg_lift`、`rotate` 不能通过 `--real-run-confirm` 进入真实机器人执行。

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

真实双机器人执行已经通过 `dual_robot_demo --real-run-confirm` 开放，后续仍应把更复杂的 sequence/time-line 层独立出来。

## 测试记录

双机器人 Demo 测试结果统一记录到 [dual-robot-test-report.md](dual-robot-test-report.md)。

每次真实测试建议用 `tee` 保存完整终端日志，再用 `scripts/dual_robot_log_to_report.py` 生成可粘贴的 Markdown 表格行。测试记录只基于已有日志整理，不改变控制逻辑，也不会发送机器人控制命令。
