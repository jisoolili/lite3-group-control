# 扩展到 10 台机器人

当前目标：

```text
2 robots
```

长期目标：

```text
10 robots
```

## 分离原则

必须把以下概念分离：

- Robot ID
- Action
- Execution Time
- Transport

`GroupController` 不应该依赖具体 IP。它接收 Robot ID 和动作，再由下层解析机器人配置和通信方式。

## Robot ID

使用稳定逻辑 ID：

```text
Robot_01
Robot_02
Robot_03
Robot_04
Robot_05
Robot_06
Robot_07
Robot_08
Robot_09
Robot_10
```

## Transport 要求

每台机器人应独立维护：

- 目标 IP
- 目标控制端口
- 本地 RobotState 端口
- 健康状态
- 超时状态

单台机器人离线不应该影响其他机器人安全处理。

## 扩展步骤

1. 验证 `Robot_01` 单机器人动作。
2. 验证 `Robot_02` 单机器人动作。
3. 验证 `Robot_01 + Robot_02` dry-run 时间线。
4. 在同一组控 API 后接入真实 Transport。
5. 验证双机器人真实同步动作。
6. 以配置方式加入 `Robot_03` 到 `Robot_05`。
7. 以配置方式加入 `Robot_06` 到 `Robot_10`。
8. 用 dry-run 日志验证 Wave 时序。
9. 以保守速度验证真实 Wave。
10. 记录每台机器人的稳定性和时序结果。
