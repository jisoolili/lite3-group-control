# 单机器人动作

当前单机器人动作开发使用配套 `transfer` 包和 `lite3_stand_demo` 可执行程序。

## 已实现动作

当前单机器人动作 demo 已实现：

- `stand`
- `squat-1-3`
- `squat-2-3`
- `squat-full`
- `squat-sequence`
- `fast-squat`

Robot A 已经验证过：

- `stand`
- `squat-1-3`
- `squat-2-3`
- `squat-full`

Dry-run 已验证：

- `fast-squat`

## 当前参数

```text
STAND:
hip = 0 deg
thigh = -42 deg
knee = 78 deg

PreStand:
hip = 0 deg
thigh = -70 deg
knee = 150 deg

SQUAT_1_3:
thigh = -52.810854 deg
knee = 105.799339 deg
target height = 0.269377 m

SQUAT_2_3:
thigh = -61.810822 deg
knee = 128.942113 deg
target height = 0.198134 m

SQUAT_FULL:
target height ~= 0.126891 m
```

控制参数：

```text
kp = 60
hold kp = 80
kd = 0.7
control cycle = 1 ms
```

## Dry Run

```bash
ros2 run transfer lite3_stand_demo --dry-run --action stand
ros2 run transfer lite3_stand_demo --dry-run --action squat-1-3
ros2 run transfer lite3_stand_demo --dry-run --action squat-2-3
ros2 run transfer lite3_stand_demo --dry-run --action squat-full
```

## 真实机器人

执行真实机器人动作前必须先阅读 `docs/safety.md`。

Robot A 有线 stand 示例命令：

```bash
ros2 run transfer lite3_stand_demo \
  --real-run-confirm \
  --action stand \
  --robot-ip 192.168.1.120 \
  --robot-port 43893
```

一次只执行一个动作。如果出现 RobotState timeout，立即停止后续动作并保留日志。不要第一时间通过放宽 watchdog 或修改增益来掩盖通信问题。

## 规划动作

- `Height`
- `Sway`
- `Pitch`
- `LegLift`
- `Rotate`
- `Wave`

## FastSquat

`fast-squat` 是当前新增的单机器人标准动作，作为后续 Wave 声浪动作的基础单元之一。

动作顺序：

```text
STAND_HOLD -> SQUAT_FULL -> STAND
```

参数：

```text
下蹲过渡时间 = 0.8 s
最低位保持 = 0.4 s
回正过渡时间 = 0.8 s
回正保持 = 1.0 s
```

动作角度复用已验证参数：

```text
STAND:
hip = 0 deg
thigh = -42 deg
knee = 78 deg

SQUAT_FULL:
hip = 0 deg
thigh = -70 deg
knee = 150 deg
```

群控接口名称：

```text
fast_squat -> transfer/lite3_stand_demo --action fast-squat
```

当前状态：

```text
Implemented: yes
Dry-run: PASS
Real robot validation: PASS
```

真实验证结果：

```text
Robot: Robot A
Network: wired
Command: lite3_stand_demo --real-run-confirm --action fast-squat --robot-ip 192.168.1.120 --robot-port 43893
Result: PASS
RobotState timeout: no
Exit code: 0
send_interval_ms min=0.112388 avg=1.00013 max=2.11284
max_abs_joint_error=0.133695
```
