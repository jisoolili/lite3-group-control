# Single Robot Actions

Single-robot action development currently uses the companion `transfer` package and `lite3_stand_demo` executable.

## Implemented Actions

Implemented in the current single-robot action demo:

- `stand`
- `squat-1-3`
- `squat-2-3`
- `squat-full`
- `squat-sequence`

Validated on Robot A previously:

- `stand`
- `squat-1-3`
- `squat-2-3`
- `squat-full`

## Current Parameters

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

Control parameters:

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

## REAL ROBOT

Read `docs/safety.md` before any real command.

Example Robot A wired stand command:

```bash
ros2 run transfer lite3_stand_demo \
  --real-run-confirm \
  --action stand \
  --robot-ip 192.168.1.120 \
  --robot-port 43893
```

Run one action at a time. If RobotState timeout appears, stop and preserve logs. Do not loosen watchdogs or change gains as a first response.

## Planned Actions

- `Height`
- `FastSquat`
- `Sway`
- `Pitch`
- `LegLift`
- `Rotate`
- `Wave`
