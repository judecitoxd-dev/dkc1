# Progress accounting

## Current total: 3%

The total is milestone-weighted so it cannot be inflated by generating thousands of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 20% | 3.00% |
| Portable gameplay/system C | 45% | 3% | 1.35% |
| Graphics, camera, tilemaps, widescreen | 15% | 1% | 0.15% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 0% | 0.00% |
| Validation and packaging | 5% | 2% | 0.10% |

Weighted pipeline foundation: **9.60%**. The public headline remains a conservative **3%** because the executable still has no playable scene.

## Completed in this stage

- Resolved the NMI callback dispatch at `$80:A98B` for three confirmed targets.
- Identified NMI as a cooperative frame scheduler and modeled it in C.
- Identified `$0508/$050A/$050C` as the current state callback, table base, and sequence index.
- Reimplemented the state-sequence selection logic at `$80:81C7-$80:81E0`.
- Followed five unique initial state callbacks from two confirmed tables.
- Expanded the graph to 115 routine entries and 3,849 unique instruction addresses.
- Reimplemented the boot-control state at `$00:BA43-$00:BA8D`.
- Reimplemented the initial PPU configuration and seven VRAM transfer requests.
- Expanded automated validation from three to seven passing tests.

## Next measurable targets

- Resolve jump tables at `$80:9729/$80:972D` and the banked dispatch at `$80:86E0`.
- Name the state routines at `$80:822C`, `$80:8279`, `$80:82D2`, `$80:9723`, and `$80:9ADA` by behavior.
- Determine the host-side object list and camera state touched by the initial sequence.
- Build a portable frame scheduler that invokes translated C callbacks instead of SNES addresses.
- Begin a software tilemap/PPU abstraction only after those state interfaces are stable.
