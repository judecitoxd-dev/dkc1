# Progress accounting

## Current total: 16%

The total is milestone-weighted so it cannot be inflated by generating thousands of meaningless C stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 46% | 6.90% |
| Portable gameplay/system C | 45% | 27% | 12.15% |
| Graphics, camera, tilemaps, widescreen | 15% | 26% | 3.90% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 18% | 1.26% |
| Validation and packaging | 5% | 16% | 0.80% |

Weighted pipeline foundation: **30.01%**. The public headline remains a conservative **16%** because no complete original-compatible level is playable yet.

## What counts as converted

A routine or host system counts only when its inputs, outputs, side effects, portable implementation, validation, and runtime connection are present.

## Completed in this five-point stage

### 12% — exact local ROM access

- Added file loading and owned-buffer cleanup.
- Added checked HiROM conversion for mirrored upper-half and full-bank mappings.
- Added checked 8/16/24-bit reads and bounded copies.
- Added exact cartridge-header validation for the supported USA Rev. 2 dump, including checksum/complement validation.

### 13% — SNES graphics and palette decoding

- Added standard 32-byte planar 4bpp tile decoding.
- Added multi-tile decoding and direct decoding from a local HiROM address.
- Added SNES 15-bit color conversion and palette decoding.
- No original graphics or palettes are committed.

### 14% — reconstructed terrain-cell format

- Identified `$81:8000` as the object terrain-height entry and `$81:800D-$81:8167` as its main cell query.
- Reconstructed the column-major, vertically reversed 16-row map layout.
- Reconstructed 32-pixel block flipping, 16-pixel half selection, descriptor flipping, shape ids, and material flags.
- Translated all 63 valid height transforms selected by the table at `$81:84C9`, including the curve table at `$81:86E5`.

### 15% — host RGBA and asset probe

- Added indexed-to-RGBA conversion, clipped rectangle drawing, and binary PPM output.
- Added `dk1_asset_probe`, which validates the local ROM and renders 16 tiles with a user-selected palette.
- Verified the complete ROM-to-PPM route locally without committing the generated image.

### 16% — collision/input validation scene

- Added a small host-only actor used to exercise directional input, gravity, jump input, terrain floor lookup, landing, and widescreen-compatible presentation.
- The actor is explicitly a validation harness, not a claim that original player physics are translated.
- Expanded automated validation from 18 to 23 tests.

## Next measurable targets

- Resolve the level-specific pointers loaded by `$81:8C67-$81:8CAF` and bind an actual level collision map from the local ROM.
- Translate the full neighboring-cell behavior and material side effects of `$81:800D`.
- Identify and translate the compression formats used by the initial VRAM upload sources.
- Translate a common actor callback and its real movement/collision fields.
- Add a real PC window/input backend and display the first decoded original background layer.
