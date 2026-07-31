# Progress accounting

## Current total: 40%

The total is milestone-weighted so it cannot be inflated by generated stubs. Forty percent refers to completed engineering infrastructure and translated systems, not forty percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 62% | 9.30% |
| Portable gameplay/system C | 45% | 55% | 24.75% |
| Graphics, camera, tilemaps, widescreen | 15% | 68% | 10.20% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 30% | 2.10% |
| Validation and packaging | 5% | 45% | 2.25% |

Weighted pipeline foundation: **53.60%**. The public headline remains **40%** because no original-compatible level is yet playable from start to finish.

## Completed in the 26-to-40% stage

### 27% — scene initializer recipes

- Classified the 31 unique bank-`$B9` initializer callbacks used by the 230 level/location ids.
- Recorded PPU preset, terrain profile, primary VRAM package, palette uploads, and the special inverted-palette path.

### 28% — all level ids bound to recipes

- Reads the real callback selected by `$B9:801E` for every level id.
- All 230 ids resolve to one of the 31 translated recipes.

### 29% — package sequencing

- Reconstructed the primary/shared package order used during scene initialization.
- Preserved alternate special-state packages for ids `$006C`, `$00D2`, and `$00D3`.

### 30% — complete CGRAM model

- Added all 256 SNES palette entries.
- Translated the bank-`$B9` palette transfer count and destinations.
- Supports normal and inverted-byte palette upload behavior.

### 31% — exact per-level camera bounds

- Translated the pointer table beginning at `$BC:8000`.
- All level ids can load their original horizontal minimum and maximum.

### 32% — PPU background layouts

- Derives mode, layer enable state, screen base, character base, map dimensions, tile size, and color depth from each reconstructed PPU preset.

### 33% — 2bpp and 8bpp graphics

- Extended the existing 4bpp decoder with standard SNES 2bpp and 8bpp planar formats.
- BG3 and higher-color scene data can now use their native depth.

### 34% — complete scene-memory bootstrap

- One API now reconstructs recipe, package plan, PPU, terrain, camera, VRAM, and CGRAM from a level id and the local ROM.

### 35% — VRAM background renderer

- Reads SNES tilemap words from reconstructed VRAM.
- Supports 32x32, 64x32, 32x64, and 64x64 maps, screen wrapping, flips, palette selection, scrolling, and transparent color zero.

### 36% — diagnostic PPU compositor

- Composes backdrop and reconstructed mode-1 BG2/BG3 layers.
- Keeps the accuracy boundary explicit: full priority, windows, and color math remain pending.

### 37% — corrected visual streaming map

- Corrected `$81:8CEF` visual cell addressing to `row * 128 + column * 2`.
- Separated visual streaming order from the column/reversed-Y collision access.
- Added the zero-cell blank sentinel discovered through real-ROM validation.

### 38% — widescreen scene view

- Builds a camera-relative BG1 strip for arbitrary host viewport widths.
- Applies original per-level camera bounds while allowing 384-pixel and wider presentation.

### 39% — scene runtime and universal preview fallback

- Added deterministic camera stepping for host input.
- `dk1_scene_probe` uses direct streaming-map composition where available and reconstructed VRAM when metatile definitions are not directly ROM-readable.
- All 31 unique recipes produced local previews: 23 stream-map and eight VRAM fallback.

### 40% — whole-cartridge scene validation

- Added deterministic FNV-1a signatures over recipe, PPU, terrain, camera, VRAM, and CGRAM state.
- Local validation reconstructed 230/230 scenes with zero failures.
- Supported-ROM aggregate signature: `6F88519C029414AB`.
- Expanded configured validation from 33 to 46 tests; all 13 newly introduced tests pass locally with warnings treated as errors.

## Next measurable targets

- Translate a common original player/actor callback and its animation/OAM output.
- Complete neighboring-cell and material side effects of `$81:800D`.
- Reproduce dynamic BG1 streaming rather than only initial/static reconstruction.
- Implement exact SNES layer priority, windows, color math, and animated palettes.
- Add a real PC window, keyboard/gamepad backend, audio, saves, and menus.
- Compare native frame traces against an emulator reference before calling the first level playable.
