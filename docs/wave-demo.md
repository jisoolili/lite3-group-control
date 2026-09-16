# Wave Demo

`Wave` is the planned stage "sound wave" action.

The single-robot motion idea is:

```text
H2 -> H0 -> H4 -> H2
```

The multi-robot choreography should be parameterized:

```text
Wave(
    group,
    direction,
    interval,
    repetitions,
    speed
)
```

## Design Principles

- Wave must not be hard-coded for only two robots.
- Robot timing should be defined by sequence/timeline data.
- Each robot uses the same reusable action definition.
- Per-robot delay creates the wave shape.

## Example Timeline

```text
Robot_01 wave start 0.00s
Robot_02 wave start 0.25s
Robot_03 wave start 0.50s
...
Robot_10 wave start 2.25s
```

Current status: planned. The dry-run sequence infrastructure exists, but real Wave action execution is not implemented yet.
