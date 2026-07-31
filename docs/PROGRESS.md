# Progress accounting

## Current total: 11%

The total is milestone-weighted so it cannot be inflated by generating thousands of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 42% | 6.30% |
| Portable gameplay/system C | 45% | 18% | 8.10% |
| Graphics, camera, tilemaps, widescreen | 15% | 15% | 2.25% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 10% | 0.70% |
| Validation and packaging | 5% | 10% | 0.50% |

Weighted pipeline foundation: **22.85%**. The public headline is a conservative **11%** because no original-compatible playable level is loaded yet.

## What counts as converted

A routine or host system counts only when its inputs, outputs, side effects, portable implementation, validation, and runtime connection are present.

## Completed in this five-point stage

### 7% — controller input

- Translated `$00:C180-$00:C1AF` held/new-press calculation.
- Translated `$00:C1B2-$00:C20A` active-controller selection and shared-input merging.
- Added SNES button masks and deterministic tests.

### 8% — host callback registry

- Added a 24-bit source-address to C-function registry.
- Supports replacement, lookup, execution, missing-callback accounting, and failure accounting.
- Keeps every translated callback traceable to its original SNES address.

### 9% — object execution

- Connected the translated `$BF:8000` object scheduler to host callbacks.
- Added portable object positions, velocities, flags, frame numbers, and per-slot invocation contexts.
- Separately counts executed, untranslated, and failed callbacks.

### 10% — host rendering foundation

- Added camera-relative object projection and viewport culling.
- Added stable ordering from the confirmed object priority field.
- Added a wrapped 8x8 indexed tilemap renderer that supports viewports wider than the SNES display without embedding original assets.
- Expanded PPU scroll support to three confirmed profiles from `$80:8973`, `$80:8CA0`, and `$80:8DF0`.

### 11% — integrated native frame

- One host frame now updates input, selects a controller, runs original object scheduling, invokes translated callbacks, computes PPU scroll, and builds the visible render queue.
- The diagnostic executable runs this path with a translated C callback.
- Expanded automated validation from 12 to 18 tests.

## Next measurable targets

- Translate shared collision and terrain-query routines used by player and object callbacks.
- Decode level tilemap layouts from the user's local ROM at runtime without committing assets.
- Translate enough object callbacks to animate a controllable actor.
- Add a real PC window/input backend and indexed-to-RGBA presentation.
- Validate frame traces against the SNES build before calling the first scene playable.
