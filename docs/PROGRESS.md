# Progress accounting

## Current total: 5%

The total is milestone-weighted so it cannot be inflated by generating thousands
of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 40% | 6.00% |
| Portable gameplay/system C | 45% | 7% | 3.15% |
| Graphics, camera, tilemaps, widescreen | 15% | 4% | 0.60% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 0% | 0.00% |
| Validation and packaging | 5% | 4% | 0.20% |

Weighted pipeline foundation: **14.95%**. The public headline remains a
conservative **5%** because the executable still has no playable scene.

## What counts as converted

A routine counts only when:

1. Its inputs, outputs, RAM fields, and side effects are documented.
2. Portable C reproduces its behavior without embedding original machine-code bytes.
3. A test or trace comparison validates it.
4. It is connected to the host runtime.

## Completed in this stage

- Confirmed 58 object slots represented by even WRAM offsets from `$0000` through `$0072`.
- Confirmed the primary update pool at slots `1-25` and the secondary pool at slots `26-57`.
- Confirmed object type, position, flags, script, and callback parallel arrays.
- Reimplemented 24-bit object callback composition from `$130D/$1341`.
- Added the complete 122-record `$BF:817C` object-type dispatch table to portable C.
- Preserved the observed object ordering value (`attributes & $003F`) and special-pass bit (`$0080`).
- Confirmed `$088B/$0895` as the camera X/Y origin from object rendering subtraction.
- Confirmed horizontal limits `$1B23/$1B25` and the original vertical clamp behavior.
- Added a configurable host viewport rectangle so widescreen dimensions remain separate from original simulation state.
- Expanded automated validation from eight to ten tests.

## Next measurable targets

- Translate the primary object iteration policy at `$BF:8000-$BF:80F2` into host callback invocation.
- Replace translated object callback addresses with stable host callback identifiers.
- Identify collision and terrain-query fields used by the shared frame callbacks.
- Determine tilemap streaming coordinates derived from camera X/Y.
- Render the first static level background before enabling gameplay movement.
- Continue classifying the remaining dynamic script callback sites without guessing targets.
