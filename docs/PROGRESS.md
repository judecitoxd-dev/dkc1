# Progress accounting

## Current total: 6%

The total is milestone-weighted so it cannot be inflated by generating thousands
of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 40% | 6.00% |
| Portable gameplay/system C | 45% | 9% | 4.05% |
| Graphics, camera, tilemaps, widescreen | 15% | 6% | 0.90% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 0% | 0.00% |
| Validation and packaging | 5% | 5% | 0.25% |

Weighted pipeline foundation: **16.20%**. The public headline remains a
conservative **6%** because the executable still has no playable scene.

## What counts as converted

A routine counts only when:

1. Its inputs, outputs, RAM fields, and side effects are documented.
2. Portable C reproduces its behavior without embedding original machine-code bytes.
3. A test or trace comparison validates it.
4. It is connected to the host runtime.

## Completed in this stage

- Translated the secondary object loop at `$BF:815E-$BF:8175`.
- Translated the primary object selection routine at `$BF:8000-$BF:80F4`.
- Preserved secondary-before-primary frame ordering.
- Preserved the negative `$1929` primary-delay behavior.
- Implemented normal, restricted, focus, and special primary passes.
- Preserved special handling for types `$17`, `$26`, `$31`, and `$45`.
- Preserved the type `$26` state checks against `$1029` values `0` and `5`.
- Connected selected type ids to their confirmed bank-`$BF` callbacks.
- Translated the BG1/BG2 half-speed parallax output at `$80:8973-$80:89B3`.
- Exposed object pass and PPU scroll diagnostics in the host executable.
- Expanded automated validation from ten to twelve tests.

## Next measurable targets

- Translate the first shared object callbacks reached by the most common type records.
- Add a host callback registry that replaces translated SNES callback addresses with C functions.
- Identify collision and terrain-query fields used by `$80:9738` and related frame paths.
- Determine tilemap streaming coordinates and VRAM destinations derived from camera movement.
- Render the first static tilemap layer before enabling gameplay movement.
- Continue classifying dynamic script callback sites without guessing targets.
