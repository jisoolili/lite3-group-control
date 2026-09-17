# Lite3 单机连接与动作验证

## 1. 验证范围

当前实际项目只覆盖两台 Lite3：

- `Robot_A`
- `Robot_B`

3 到 10 台机器人只作为后续架构扩展规划，本文件不记录 3 到 10 台实际验证结果。

本轮只整理单机连接验证、单体动作验证、测试接口和验证记录。不修改官方 Lite3 MotionSDK、`jy_exe`、`network.toml`、网络配置、控制周期、watchdog、kp/kd 或已经验证的动作参数。

## 2. Robot_A / Robot_B 连接信息

| Robot ID | IP | 控制端口 | RobotState 本地端口 | 通信方式 | 网络连接 | 动作接口 | 当前状态 |
|---|---:|---:|---:|---|---|---|---|
| `Robot_A` | `192.168.1.120` | `43893` | `43897` | UDP | 有线 | `transfer/lite3_stand_demo` | 已完成单机真实动作验证 |
| `Robot_B` | TBD | `43893` | TBD | UDP | 待现场确认 | `transfer/lite3_stand_demo` | 待现场连接验证 |

状态说明：

- `network_online`：仅表示网络链路可达，不等于真实动作验证通过。
- `control_ready`：表示目标 IP、端口、RobotState 回传和控制链路已满足动作测试前置条件。
- `action_verified`：表示对应机器人已经完成明确动作的真实机器人验证。

当前已知：

| Robot ID | network_online | control_ready | action_verified | 备注 |
|---|---|---|---|---|
| `Robot_A` | ONLINE | READY | 部分动作 REAL PASS | 有线链路与 RobotState 已验证稳定 |
| `Robot_B` | PENDING | PENDING | PENDING | 不使用 Robot_A 结果推断 Robot_B |

## 3. 单机连接验证

当前代码已经具备以下抽象：

- `RobotConfig`：保存 `robot_id`、IP、控制端口、RobotState 本地端口、通信方式、网络方式、动作接口和动作验证标记。
- `RobotManager`：按 `robot_id` 注册、查询和隔离不同机器人。
- `ActionManager`：按 action string 注册并分发动作。
- `GroupController`：按 `robot_id + action` 形成单机器人或双机器人 dry-run 调度。
- `dual_robot_demo`：只允许 `--dry-run`，不会发送真实控制命令。

单机器人 dry-run 示例：

```bash
ros2 run group_control dual_robot_demo --robot Robot_A --action stand --dry-run
ros2 run group_control dual_robot_demo --robot Robot_B --action stand --dry-run
ros2 run group_control dual_robot_demo --robot Robot_A --action fast_squat --dry-run
ros2 run group_control dual_robot_demo --robot Robot_B --action fast_squat --dry-run
```

日志必须包含：

```text
[Robot_A] action=stand target=192.168.1.120:43893
[Robot_B] action=stand target=<Robot_B IP>:43893
```

当前 `Robot_B` IP 仍为占位值，真实动作前必须替换为现场确认地址，并单独完成连接验证。

## 4. Robot_A / Robot_B 身份映射

身份映射原则：

- `Robot_A` 和 `Robot_B` 是逻辑 ID，不是 IP 地址。
- 动作调度必须先通过 `RobotManager` 找到对应 `RobotConfig`。
- `ActionManager` 不硬编码机器人 IP。
- 日志必须同时显示 `robot_id`、IP、端口和动作名。
- 不允许因为默认 IP 或默认机器人导致动作发错目标。

当前 dry-run 调度接口：

```bash
ros2 run group_control dual_robot_demo --robot Robot_A --action stand --dry-run
ros2 run group_control dual_robot_demo --robot Robot_B --action stand --dry-run
```

## 5. 单体动作验证

验证状态统一使用：

- `REAL PASS`
- `DRY-RUN PASS`
- `PENDING REAL TEST`
- `NOT IMPLEMENTED`

### 5.1 机身高度

已有实测高度：

| 档位 | 定义 | 实测高度 | 是否验证 | 备注 |
|---|---|---:|---|---|
| H0 | 最低 / `squat-full` | `0.126891 m` | REAL PASS | Robot_A 已真实验证 |
| H1 | 低位 | TBD | PENDING REAL TEST | 不虚构高度 |
| H2 | 正常 / `stand` | `0.340620 m` | REAL PASS | Robot_A 已真实验证 |
| H3 | 高位 | TBD | PENDING REAL TEST | 不虚构高度 |
| H4 | 最高安全位 | TBD | NOT IMPLEMENTED | 需要先完成安全边界定义 |

当前可确认：

```text
STAND = 0.340620 m
FULL = 0.126891 m
range = 0.213729 m
```

待实测字段：

- 最低安全高度：当前以 H0 / `squat-full` 作为已验证最低动作记录。
- 最高安全高度：TBD。
- 最大高度差：当前已知 `0.213729 m`，仅覆盖 H0 到 H2。
- 升降速度：TBD。
- 连续升降稳定性：TBD。

### 5.2 快速蹲起

已实现动作：

```text
fast_squat -> lite3_stand_demo --action fast-squat
```

已验证流程：

```text
正常 -> 最低 -> 正常
```

参数：

```text
squat = 0.8 s
lowest hold = 0.4 s
return = 0.8 s
final stand = 1.0 s
```

Robot_A 真实验证记录：

```text
Result: REAL PASS
avg send interval ~= 1.00013 ms
max_abs_joint_error = 0.133695
code = 0
timeout = none
```

未完成：

- 正常 -> 最低 -> 最高 -> 正常：NOT IMPLEMENTED。
- 连续 2 次真实测试：PENDING REAL TEST。
- 连续 4 次真实测试：PENDING REAL TEST。
- 是否适合舞台表演：PENDING HUMAN REVIEW。

### 5.3 左右摆动

动作：

```text
sway
```

流程：

```text
左倾 -> 回正 -> 右倾 -> 回正
```

当前参数：

| 字段 | 当前值 |
|---|---|
| 左倾角度 | `hip_offset = 3 deg` |
| 右倾角度 | `hip_offset = 3 deg` |
| 单次过渡时间 | `0.7 s` |
| 单次保持时间 | `0.35 s` |
| 默认周期 | `2` |
| 最大安全角度 | TBD |
| 推荐测试角度 | `3 deg` dry-run 起点 |
| 稳定性 | PENDING REAL TEST |
| 是否真实验证 | 否 |

验证状态：`DRY-RUN PASS`。

### 5.4 前后俯仰

动作：

```text
pitch
```

流程：

```text
前压 -> 回正 -> 后仰 -> 回正
```

当前参数：

| 字段 | 当前值 |
|---|---|
| 前压角度 | `thigh_offset = 4 deg`, `knee_offset = 6 deg` |
| 后仰角度 | `thigh_offset = 4 deg`, `knee_offset = 6 deg` |
| 单次过渡时间 | `0.7 s` |
| 单次保持时间 | `0.35 s` |
| 最大安全角度 | TBD |
| 稳定性 | PENDING REAL TEST |
| 是否真实验证 | 否 |

验证状态：`DRY-RUN PASS`。

### 5.5 抬腿

动作：

```text
leg_lift -> lite3_stand_demo --action leg-lift
```

当前代码状态：

- 已建立通用 `leg_lift` action。
- 当前 CLI dry-run 默认只预览 `LF` 单腿抬起。
- 尚未实现 `left_front / right_front / alternate / hold` 的 CLI 参数化真实动作。

当前参数：

| 字段 | 当前值 |
|---|---|
| leg | `LF` |
| mode | `lift` |
| thigh | `-54 deg` |
| knee | `110 deg` |
| 过渡时间 | `0.7 s` |
| 保持时间 | `0.35 s` |
| 稳定性 | PENDING SAFETY VALIDATION |
| 是否真实验证 | 否 |

任务书要求与当前状态：

| 测试项 | 当前状态 | 备注 |
|---|---|---|
| 左前腿 | DRY-RUN PASS | 当前默认 `LF` |
| 右前腿 | NOT IMPLEMENTED | 需要先补参数化接口 |
| 左右交替 | NOT IMPLEMENTED | 需要先补参数化接口 |
| 单腿保持 | PENDING REAL TEST | 高稳定性风险，需人工安全确认 |

真实测试状态：`PENDING SAFETY VALIDATION`。

### 5.6 旋转

动作：

```text
rotate
```

当前结论：

```text
旋转动作接口已建立，真实旋转控制待进一步接入/验证。
```

当前 `rotate` 是 dry-run / action placeholder，不是已经完成的真实原地旋转控制。

测试字段：

| 字段 | 当前值 |
|---|---|
| 目标角度 | 90 / 180 / 360 待实测 |
| 实际角度 | TBD |
| 方向 | 正向 / 反向待实测 |
| 执行时间 | TBD |
| 方向误差 | TBD |
| 稳定性 | PENDING REAL TEST |
| 是否真实验证 | 否 |

验证状态：`DRY-RUN PASS`。

## 6. 单体动作测试表

| 动作名称 | 动作效果 | 参数 | 执行时间 | 稳定性 | 验证状态 | 备注 |
|---|---|---|---|---|---|---|
| `stand` | 正常站立 | STAND 高度 `0.340620 m` | 已验证参数 | 已验证 | REAL PASS | Robot_A |
| `squat-1-3` | 低幅下蹲 | thigh `-52.810854 deg`, knee `105.799339 deg` | 已验证参数 | 已验证 | REAL PASS | Robot_A |
| `squat-2-3` | 中幅下蹲 | thigh `-61.810822 deg`, knee `128.942113 deg` | 已验证参数 | 已验证 | REAL PASS | Robot_A |
| `squat-full` | 最低位下蹲 | height `0.126891 m` | 已验证参数 | 已验证 | REAL PASS | Robot_A |
| `fast_squat` | 正常 -> 最低 -> 正常 | 0.8 / 0.4 / 0.8 / 1.0 s | 约 `3.0 s` | Robot_A 已验证 | REAL PASS | 不包含连续 2 次 / 4 次 |
| `sway` | 左右摆动 | `hip_offset = 3 deg` | dry-run 轨迹 | 待实测 | DRY-RUN PASS | 未真实验证 |
| `pitch` | 前后俯仰 | `thigh_offset = 4 deg`, `knee_offset = 6 deg` | dry-run 轨迹 | 待实测 | DRY-RUN PASS | 未真实验证 |
| `leg_lift` | 单腿抬腿 | `LF`, thigh `-54 deg`, knee `110 deg` | dry-run 轨迹 | 待安全验证 | DRY-RUN PASS | 高稳定性风险 |
| `rotate` | 原地旋转接口占位 | `hip_offset = 3 deg` | dry-run 轨迹 | 待实测 | DRY-RUN PASS | 真实旋转未接入 |

## 7. 已完成验证

REAL PASS：

- Robot_A `stand`
- Robot_A `squat-1-3`
- Robot_A `squat-2-3`
- Robot_A `squat-full`
- Robot_A `fast_squat`

DRY-RUN PASS：

- `sway`
- `pitch`
- `leg_lift`
- `rotate`
- `Robot_A -> stand`
- `Robot_B -> stand`
- `Robot_A -> fast_squat`
- `Robot_B -> fast_squat`

## 8. 待现场验证

- Robot_B IP、RobotState 回传端口和控制链路。
- Robot_B `stand` 真实机器人验证。
- Robot_B `fast_squat` 真实机器人验证。
- `sway` 真实机器人低幅验证。
- `pitch` 真实机器人低幅验证。
- `leg_lift` 安全评审后真实验证。
- `rotate` 真实旋转控制接口接入与验证。
- H1 / H3 / H4 高度标定。
- `fast_squat` 连续 2 次、连续 4 次真实测试。
- 正常 -> 最低 -> 最高 -> 正常流程设计和验证。

## 9. 安全说明

- 本文件不授权执行真实机器人动作。
- 禁止在未确认现场安全条件时执行 `--real-run-confirm`。
- 禁止把 Wi-Fi 抖动或网络未稳定误判为动作代码问题。
- 禁止使用 Robot_A 的真实验证结果推断 Robot_B 已验证。
- `leg_lift` 和 `rotate` 必须先完成安全评审，再进入真实机器人测试。
