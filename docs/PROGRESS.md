# Progress accounting

## Current total: 21%

The total is milestone-weighted so it cannot be inflated by generated stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 50% | 7.50% |
| Portable gameplay/system C | 45% | 35% | 15.75% |
| Graphics, camera, tilemaps, widescreen | 15% | 36% | 5.40% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 22% | 1.54% |
| Validation and packaging | 5% | 22% | 1.10% |

Weighted pipeline foundation: **36.29%**. The public headline remains **21%** because a complete original-compatible level is still not playable.

## Completed in this five-point stage

### 17% — exact terrain/streaming configuration records

- Translated the 14 split records consumed by `$81:8C67-$81:8CAF`.
- Each portable record exposes the map pointer, visual metatile bank/base, collision-descriptor pointer, stream destination, collision block count, and terrain callback.
- Preserved the original zero-bank fallback to bank `$80`.

### 18% — ROM-backed terrain binding

- Added terrain sampling that reads map cells and block-half descriptors directly from the user's ROM.
- Reuses the previously translated 63 slope transforms and optional shape flags.
- Supports floor searches without copying or committing level data.

### 19% — visual metatile expansion

- Translated the visual block expansion behavior visible at `$81:8CEF-$81:8DDF`.
- Expands one 32x32 map cell into sixteen SNES tilemap words.
- Preserves horizontal and vertical map-cell flips, source order reversal, and tile flip-bit XOR behavior.
- Corrected the model after real-ROM validation showed `$DB` limits collision descriptors, not visual metatile ids.

### 20% — complete map-column expansion

- Added top-to-bottom map-cell reads for the column-major, vertically reversed layout.
- Expands any requested range of 32-pixel columns into a 4x64 8x8-tile strip per column.
- Keeps tile numbers, palette bits, priority, and flip flags intact for future rendering.

### 21% — local level diagnostic tool

- Added `dk1_level_probe` for supported local ROMs.
- Produces a 512-pixel-tall PPM using deterministic colors for actual tilemap words.
- Overlays the reconstructed collision floor in red.
- Verified profile zero locally with an eight-column, 256x512 output.
- Expanded automated validation from 23 to 28 tests.

## Next measurable targets

- Trace the exact level-id to terrain-profile selection path.
- Translate compressed graphics and VRAM package formats used by the chosen profile.
- Bind real tile graphics and palettes to the expanded tilemap words.
- Complete neighboring-cell and material side effects of `$81:800D`.
- Translate one common original actor callback against the bound terrain view.
