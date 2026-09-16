# Robot Network

Lite3 hardware is documented with two host roles:

## Hardware Roles

`运动主机`

- Runs `jy_exe`.
- Owns the low-level motion control interface.
- Receives UDP control commands.
- Sends RobotState.

`感知主机`

- Runs ROS2 and perception processes.
- Handles lidar, camera, and `jetson2motion` style integration.
- Is not the main target for the current group action baseline.

Current group action development primarily depends on the `运动主机` motion-control link.

## Development Network

Use wired networking for action development whenever possible:

- lower latency
- stable RobotState timing
- easier debugging
- less ambiguity between motion code and wireless issues

Wireless networking is a separate debugging track.

## Current Wired Baseline

The latest Robot A wired RobotState baseline:

```text
Ubuntu development machine:
192.168.1.99
interface: enp2s0

运动主机:
192.168.1.120
interface: eth1
```

Measured results:

```text
ping avg < 1 ms
RobotState tcpdump max ~= 1.122 ms
Receiver callback max ~= 2.320 ms
gap_over_100ms = 0
kernel dropped = 0
```

This wired link is the current baseline for single-robot action development.

## Configuration Principle

Do not bind choreography logic to fixed IP addresses. Use:

```text
Robot ID -> robot configuration -> IP / ports
```

Example configuration lives in `config/` and `config/examples/`.
