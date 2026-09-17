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
- `sway`
- `pitch`
- `leg-lift`
- `rotate`

Robot A 已经真实验证过：

- `stand`
- `squat-1-3`
- `squat-2-3`
- `squat-full`
- `fast-squat`

Dry-run 已验证：

- `sway`
- `pitch`
- `leg-lift`
- `rotate`

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

## 表演动作 Dry-Run

以下动作已接入 CLI 与 ActionManager，但当前仅允许 dry-run。真实机器人执行会被 `lite3_stand_demo` 明确阻止，直到完成单机安全验证。

### sway

作用：小幅左右身体摆动。

接口：

```text
sway -> transfer/lite3_stand_demo --action sway
```

参数：

```text
cycles = 2
hip_offset = 3 deg
transition = 0.7 s
hold = 0.35 s
end = STAND
```

验证状态：

```text
Dry-run: PASS
Real robot validation: not run
```

### pitch

作用：小幅前后俯仰。

接口：

```text
pitch -> transfer/lite3_stand_demo --action pitch
```

参数：

```text
thigh_offset = 4 deg
knee_offset = 6 deg
transition = 0.7 s
hold = 0.35 s
end = STAND
```

验证状态：

```text
Dry-run: PASS
Real robot validation: not run
```

### leg-lift

作用：单腿低幅度抬腿预览，当前默认只抬 LF。

接口：

```text
leg_lift -> transfer/lite3_stand_demo --action leg-lift
```

参数：

```text
leg = LF
thigh = -54 deg
knee = 110 deg
transition = 0.7 s
hold = 0.35 s
end = STAND
```

验证状态：

```text
Dry-run: PASS
Real robot validation: not run
```

### rotate

作用：原地旋转/转向的姿态预览。

当前限制：仅 dry-run。完整原地旋转需要更明确的底层转向接口或专门安全验证，当前不开放真实机器人执行。

接口：

```text
rotate -> transfer/lite3_stand_demo --action rotate
```

参数：

```text
hip_offset = 3 deg
transition = 0.7 s
hold = 0.35 s
end = STAND
```

验证状态：

```text
Dry-run: PASS
Real robot validation: not run
```
