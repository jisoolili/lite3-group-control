# Architecture

The project separates robot identity, action definition, execution timing, and transport.

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
Robot_01 ... Robot_10
      |
      v
Transport
      |
      v
Lite3
```

## Layers

`RobotManager` owns robot records. It maps a logical Robot ID to robot configuration such as IP address, target port, and local RobotState port.

`ActionManager` owns named actions. An action should be reusable across robots, for example `Robot_01 + squat_low` and `Robot_07 + squat_low` should use the same action definition.

`GroupController` dispatches actions to one or more robots. It supports synchronized dispatch, different actions, delayed dispatch, and sequence dispatch.

`SequenceManager` is planned as the show timeline layer. It should define the order, timing, repetition, and speed of stage actions without knowing robot IP addresses.

`Transport` is planned as the real communication layer. It should isolate UDP/Sender/Receiver details from high-level choreography.

## Current State

Implemented:

- `RobotManager`
- `ActionManager`
- `GroupController`
- dry-run `dual_robot_demo`

Planned:

- first-class `SequenceManager`
- real Lite3 transport adapter
- multi-robot state monitoring
- parameterized Wave action
