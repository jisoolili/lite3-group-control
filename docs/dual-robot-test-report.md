# 双机器人测试记录

本文档用于长期记录 Robot_A / Robot_B 双 Lite3 群控 Demo 的真实测试、dry-run 验证和异常验证结果。

记录原则：

- 不把未执行过的测试写成 `PASS`。
- 已计划但未执行的测试统一标记为 `PENDING`。
- 执行失败但没有机器人异常的测试记录为 `FAIL`。
- 因安全条件、参数限制或程序保护而未执行机器人动作的测试记录为 `BLOCKED`。
- 区分【程序启动延迟】、【双机动作同步】和【机器人动作执行结果】。
- 当前约 `T0 -> T8 ~= 8014 ms` 的数据属于程序启动和动作前初始化链路，其中 `RobotStateInit() ~= 7000 ms`，`PRE_STAND ~= 1000 ms`，不要直接记为群控同步延迟。

## 字段说明

| 字段 | 说明 |
| --- | --- |
| 测试编号 | 例如 `TC-01` |
| 测试时间 | 建议使用 `YYYY-MM-DD HH:MM:SS` |
| 测试项目 | 例如双机器人同步控制、双机器人延迟执行 |
| Robot_A 动作 | `stand`、`squat_low`、`fast_squat` 等 |
| Robot_B 动作 | `stand`、`squat_low`、`fast_squat` 等 |
| delay | `dual_robot_demo --delay` 参数 |
| 执行模式 | `DRY_RUN` 或 `REAL_RUN_CONFIRM` |
| Robot_A 启动时间 | Robot_A 子进程 `T1_CHILD_CREATED` 或 `T2_LITE3_STAND_DEMO_START` |
| Robot_B 启动时间 | Robot_B 子进程 `T1_CHILD_CREATED` 或 `T2_LITE3_STAND_DEMO_START` |
| Robot_A 动作开始时间 | Robot_A `T8_STAND_UP_BEGIN` |
| Robot_B 动作开始时间 | Robot_B `T8_STAND_UP_BEGIN` |
| Robot_A 动作结束时间 | Robot_A 控制结束或日志中动作完成时间 |
| Robot_B 动作结束时间 | Robot_B 控制结束或日志中动作完成时间 |
| 实际执行结果 | `PASS`、`FAIL`、`BLOCKED`、`PENDING` |
| 退出码 | `dual_robot_demo` 或子进程退出码 |
| 异常信息 | 只记录实际日志中的错误 |
| 视频文件名 | 例如 `TC-01_dual_stand.mp4` |
| 备注 | 环境、姿态、安全员、测试结论等 |

## 日志留存方式

建议每次测试把完整终端日志保存到 `logs/` 目录，文件名包含测试编号：

```bash
mkdir -p logs
ros2 run group_control dual_robot_demo \
  --robot-a-action stand \
  --robot-b-action stand \
  --delay 0 \
  --real-run-confirm 2>&1 | tee logs/TC-01_dual_stand.log
```

执行后可用离线脚本生成 Markdown 表格行：

```bash
python3 scripts/dual_robot_log_to_report.py \
  --log logs/TC-01_dual_stand.log \
  --test-id TC-01 \
  --project 双机器人同步控制 \
  --result PASS \
  --video TC-01_dual_stand.mp4 \
  --notes "Robot_A / Robot_B 同步 stand 真实验证通过"
```

脚本只读取日志文件，不连接机器人，不发送控制命令。

## 一、单机器人动作验证

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-单机-01 | 待执行 | 单机器人动作验证 | stand | - | - | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | Robot_A 单机 stand 记录占位 |
| TC-单机-02 | 待执行 | 单机器人动作验证 | - | stand | - | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | Robot_B 单机 stand 记录占位 |

## 二、双机器人同步控制

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-01 | 已执行，待补具体时间 | 双机器人同步控制 | stand | stand | 0 | REAL_RUN_CONFIRM | 日志待归档 | 日志待归档 | 日志待归档 | 日志待归档 | 日志待归档 | 日志待归档 | PASS | 0 | - | 待补 | 已真实验证通过；启动链路数据约 `T0 -> T8 ~= 8014 ms`，其中 `RobotStateInit() ~= 7000 ms`，`PRE_STAND ~= 1000 ms` |
| TC-02 | 已执行，待补具体时间 | 双机器人同步控制 | squat_low | squat_low | 0 | REAL_RUN_CONFIRM | 日志待归档 | 日志待归档 | 日志待归档 | 日志待归档 | 日志待归档 | 日志待归档 | PASS | 0 | - | 待补 | 已真实验证通过 |

## 三、双机器人差异化动作

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-03 | 待执行 | 双机器人差异化动作 | squat_low | squat_high | 0 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | 不同动作同步启动 |
| TC-04 | 待执行 | 双机器人差异化动作 | stand | fast_squat | 0 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | Robot_A stand，Robot_B fast_squat |

## 四、双机器人交替/声浪

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-05 | 待执行 | 双机器人交替/声浪 | fast_squat | fast_squat | 0.25 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | 延迟形成交替效果 |
| TC-06 | 待执行 | 双机器人交替/声浪 | fast_squat | fast_squat | 0.5 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | 更明显的交替效果 |

## 五、双机器人延迟执行

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-07 | 待执行 | 双机器人延迟执行 | stand | stand | 0.25 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | 检查 delay 对动作开始时间的影响 |
| TC-08 | 待执行 | 双机器人延迟执行 | stand | stand | 0.5 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | 检查 delay 对动作开始时间的影响 |

## 六、双机器人连续 Demo

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-09 | 待执行 | 双机器人连续 Demo | stand / fast_squat | stand / fast_squat | 待定 | REAL_RUN_CONFIRM | - | - | - | - | - | - | PENDING | - | - | - | 连续 Demo 需单独确认动作间恢复状态 |

## 七、异常/独立运行验证

| 测试编号 | 测试时间 | 测试项目 | Robot_A 动作 | Robot_B 动作 | delay | 执行模式 | Robot_A 启动时间 | Robot_B 启动时间 | Robot_A 动作开始时间 | Robot_B 动作开始时间 | Robot_A 动作结束时间 | Robot_B 动作结束时间 | 实际执行结果 | 退出码 | 异常信息 | 视频文件名 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| TC-10 | 待执行 | 异常/独立运行验证 | sway | sway | 0 | REAL_RUN_CONFIRM | - | - | - | - | - | - | BLOCKED | - | 动作不在真实执行白名单 | - | `sway` 当前只允许 dry-run |
| TC-11 | 待执行 | 异常/独立运行验证 | rotate | rotate | 0 | REAL_RUN_CONFIRM | - | - | - | - | - | - | BLOCKED | - | 动作不在真实执行白名单 | - | `rotate` 当前只允许 dry-run |

