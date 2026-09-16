# 机器人网络

Lite3 硬件结构统一使用两个正式名称：

## 硬件角色

`运动主机`

- 运行 `jy_exe`。
- 负责底层运动控制。
- 接收 UDP 控制指令。
- 发送 RobotState。

`感知主机`

- 运行 ROS2 和感知相关进程。
- 负责 lidar、camera、`jetson2motion` 等感知和转发功能。
- 当前群控动作基线主要不依赖感知链路。

当前群控动作开发主要依赖 `运动主机` 的运动控制链路。

## 开发网络方案

动作开发阶段优先使用有线网络：

- 延迟更稳定
- 更适合单机器人动作开发
- 更方便区分控制问题和无线问题
- RobotState 抖动更容易定位

无线网络问题属于独立排查方向。

## 当前有线基准

Robot A 最新有线 RobotState 基准：

```text
Ubuntu 开发电脑:
192.168.1.99
接口: enp2s0

运动主机:
192.168.1.120
接口: eth1
```

已测结果：

```text
ping avg < 1 ms
RobotState tcpdump max ~= 1.122 ms
Receiver callback max ~= 2.320 ms
gap_over_100ms = 0
kernel dropped = 0
```

这条有线链路是当前单机器人动作开发基准。

## 配置原则

不要把编舞逻辑绑定到固定 IP。使用：

```text
Robot ID -> robot configuration -> IP / ports
```

示例配置位于 `config/` 和 `config/examples/`。
