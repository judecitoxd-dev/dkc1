# Progress accounting

## Current total: 2%

The total is milestone-weighted so it cannot be inflated by generating thousands of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 8% | 1.20% |
| Portable gameplay/system C | 45% | 1% | 0.45% |
| Graphics, camera, tilemaps, widescreen | 15% | 0% | 0.00% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 0% | 0.00% |
| Validation and packaging | 5% | 1% | 0.05% |

Weighted pipeline foundation: **6.70%**. The public headline remains conservative because the executable still has no gameplay loop.

## What counts as converted

A routine counts only when:

1. Its inputs, outputs, RAM fields, and side effects are documented.
2. Portable C reproduces its behavior without embedding original machine-code bytes.
3. A test or trace comparison validates it.
4. It is connected to the host runtime.

## Completed in this stage

- Recursive direct-control-flow discovery with M/X width-state tracking.
- Initial graph from reset, native NMI, and native IRQ seeds.
- 28 directly reached routine entries and 789 unique instruction addresses mapped.
- Initial symbol database for reset, DMA clears, NMI, and IRQ.
- Portable/tested C model for reset-time WRAM and VRAM clearing.

## Next measurable targets

- Resolve the first indirect NMI dispatch through the RAM callback at `$001C`.
- Label reset-time WRAM fields and PPU/DMA register writes.
- Model the boot state machine beginning at `$00:8020` and `$00:BA4E`.
- Identify camera and object-update entry points before renderer work.
