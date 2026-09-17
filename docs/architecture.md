# 系统架构

项目需要把机器人身份、动作定义、执行时间和底层通信分离，避免后续扩展到 10 台机器人时把 IP、动作和时间线混在一起。

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
Robot_A / Robot_B / future Robot_03 ... Robot_10
      |
      v
Transport
      |
      v
Lite3
```

## 分层职责

`RobotManager` 负责机器人记录。它把逻辑 Robot ID 映射到机器人配置，例如 IP、目标端口和本地 RobotState 端口。

`ActionManager` 负责动作注册和动作分发。动作必须可复用，例如 `Robot_A + squat_low` 和后续扩展机器人使用的 `squat_low` 应该使用同一个动作定义。

`GroupController` 负责把动作调度到一个或多个机器人。当前支持同步分发、不同动作分发、延迟分发和序列分发。

`SequenceManager` 是规划中的演出时间线层。它应该描述动作顺序、时间、重复次数和速度，不应该知道机器人 IP。

`Transport` 是规划中的真实通信层。它会把 UDP、Sender、Receiver 等底层细节从高层编舞逻辑中隔离出来。

## 当前状态

已实现：

- `RobotManager`
- `ActionManager`
- `GroupController`
- dry-run `dual_robot_demo`

规划中：

- 独立 `SequenceManager`
- 真实 Lite3 Transport 适配器
- 多机器人状态监控
- 参数化 Wave 声浪动作
