# Dual Robot Control

The current package supports dry-run validation for two logical robots.

## Robot IDs

Use logical IDs:

```text
Robot_01
Robot_02
```

The Robot ID is not the IP address. IP addresses belong in robot configuration.

## Dry-Run Examples

Same action on both robots:

```bash
ros2 run group_control dual_robot_demo --action squat --dry-run
```

One robot:

```bash
ros2 run group_control dual_robot_demo --robot Robot_01 --action squat_low --dry-run
```

Different actions:

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action squat_low \
  --robot-b-action squat_high \
  --dry-run
```

Delayed execution:

```bash
ros2 run group_control dual_robot_demo \
  --robot-a-action squat_low \
  --robot-b-action squat_low \
  --delay 0.25 \
  --dry-run
```

Sequence:

```bash
ros2 run group_control dual_robot_demo --sequence --dry-run
```

## API Direction

The intended high-level control shape is:

```cpp
group.execute(
    robots = {"Robot_01", "Robot_02"},
    action = "squat");
```

And for scheduled sequences:

```cpp
sequence = {
    {"Robot_01", "wave", 0.00},
    {"Robot_02", "wave", 0.25},
};
```

Real dual-robot execution is not enabled in the current demo.
