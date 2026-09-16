# Scaling To 10 Robots

Current target:

```text
2 robots
```

Long-term target:

```text
10 robots
```

## Separation Rules

Keep these concepts separate:

- Robot ID
- Action
- Execution Time
- Transport

`GroupController` should not depend on concrete IP addresses. It should receive robot IDs and actions, then use lower layers to resolve robot configuration and transport.

## Robot IDs

Use stable logical IDs:

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

## Transport Expectations

Each robot should have independent:

- target IP
- target command port
- local RobotState port
- health state
- timeout state

One offline robot should not block safe handling of the other robots.

## Scaling Steps

1. Validate Robot_01 single-robot actions.
2. Validate Robot_02 single-robot actions.
3. Validate Robot_01 + Robot_02 dry-run timeline.
4. Add real transport behind the same group API.
5. Validate two-robot real synchronized action.
6. Add Robot_03 to Robot_05 as configuration-only expansion.
7. Add Robot_06 to Robot_10.
8. Validate Wave timing with dry-run logs.
9. Validate real Wave at conservative speed.
10. Record per-robot stability and timing.
