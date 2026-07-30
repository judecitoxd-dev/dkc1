# Progress accounting

## Current total: 1%

The total is milestone-weighted so it cannot be inflated by generating thousands of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 1% | 0.15% |
| Portable gameplay/system C | 45% | 0% | 0.00% |
| Graphics, camera, tilemaps, widescreen | 15% | 0% | 0.00% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 0% | 0.00% |
| Validation and packaging | 5% | 0% | 0.00% |

Weighted raw total: **5.15%** of the pipeline foundation. Because no gameplay routine is yet semantically reimplemented, the public headline is conservatively held at **1%**.

## What counts as converted

A routine counts only when:

1. Its inputs, outputs, RAM fields, and side effects are documented.
2. Portable C reproduces its behavior without embedding original machine-code bytes.
3. A test or trace comparison validates it.
4. It is connected to the host runtime.

## Next measurable targets

- Identify reset-time subroutines and hardware register writes.
- Build a symbol database keyed by 24-bit SNES address.
- Model WRAM regions and PPU/DMA command queues.
- Reimplement the first boot state machine in portable C.
- Add a PC renderer only after the camera/tilemap interfaces stabilize.
