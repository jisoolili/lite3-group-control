# Wave 声浪演示

`Wave` 是规划中的舞台“声浪”动作。

单机器人基础动作概念：

```text
H2 -> H0 -> H4 -> H2
```

多机器人编舞应支持参数化：

```text
Wave(
    group,
    direction,
    interval,
    repetitions,
    speed
)
```

## 设计原则

- Wave 不能写死为只支持两台机器人。
- 机器人启动时间由 sequence/timeline 数据描述。
- 每台机器人复用同一个动作定义。
- 通过每台机器人的延迟形成声浪效果。

## 时间线示例

```text
Robot_01 wave start 0.00s
Robot_02 wave start 0.25s
Robot_03 wave start 0.50s
...
Robot_10 wave start 2.25s
```

当前状态：规划中。dry-run 序列调度基础已经存在，但真实 Wave 动作尚未实现。
