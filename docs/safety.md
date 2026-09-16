# Safety

Real robot execution must be deliberate and staged.

## Before Real Motion

- Confirm the area around the robot is clear.
- Confirm the floor is stable and non-slippery.
- Confirm emergency stop and remote control are available.
- Confirm only the intended robot is connected.
- Confirm no group-control real run is active.
- Confirm RobotState is stable before sending commands.
- Run dry-run first.

## Development Order

Use this order:

```text
dry-run
single robot
single robot stability check
dual robot dry-run
dual robot real validation
larger group dry-run
larger group real validation
```

## Robot Isolation

Validate Robot_01 and Robot_02 independently before any real group control. A single robot going offline must not create unsafe behavior for the rest of the group.

## Network Abnormality

If RobotState timeout, packet loss, duplicated packets, or 100 ms class gaps appear:

- stop further real actions
- preserve logs
- verify wired link
- verify `jy_exe`
- verify configuration
- do not change watchdogs, gains, or control cycle as the first response

## Prohibited During Baseline Validation

- real dual-robot control without single-robot validation
- unverified action parameters
- changing `kp`, `kd`, control cycle, or watchdog to hide a communication issue
- running real actions around people
- committing passwords, tokens, SSH keys, or private robot credentials
