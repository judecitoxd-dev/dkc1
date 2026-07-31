# Progress accounting

## Current total: 50%

The total is milestone-weighted so it cannot be inflated by generated stubs. Fifty percent refers to completed engineering infrastructure and translated systems, not fifty percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 65% | 9.75% |
| Portable gameplay/system C | 45% | 65% | 29.25% |
| Graphics, camera, tilemaps, widescreen | 15% | 82% | 12.30% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 60% | 4.20% |
| Validation and packaging | 5% | 60% | 3.00% |

Weighted pipeline foundation: **63.50%**. The public headline remains **50%** because no original-compatible level is playable from start to finish.

## Completed in the 40-to-50% stage

### 41% — SNES OAM memory model

- Added the 512-byte low table and 32-byte high table used by 128 SNES sprites.
- Decodes signed 9-bit X, Y, 9-bit tile number, palette, priority, flips, and the size-selection bit.
- Encodes portable sprite records back into the hardware layout.

### 42% — OBJ renderer

- Implements all eight OBSEL small/large size pairs.
- Reconstructs name-base and name-select character regions.
- Renders multi-tile 4bpp sprites with SNES row stepping, flips, OBJ palettes, transparency, and OAM ordering.

### 43% — sprite animation timelines

- Added bounded frame sequences with duration, tile selection, offsets, flips, looping, and completion state.
- Animation state can be applied directly to an OAM entry.

### 44% — mode-1 priority ordering

- Added explicit ordering for BG1, BG2, BG3, OBJ priorities, backdrop, and the BG3-high-priority mode.
- Provides deterministic candidate selection for future per-pixel composition.

### 45% — windows, color math, and brightness

- Added both PPU window ranges, enable/invert controls, and OR/AND/XOR/XNOR logic per layer.
- Added saturating BGR555 addition and subtraction, optional half result, and INIDISP brightness scaling.

### 46% — host keyboard input

- Added host-to-SNES button mapping and held/newly-pressed state.
- The mapping is shared by headless and windowed frontends.

### 47% — deterministic frame replay

- Records up to 4096 input frames.
- Replays from a known scene-runtime state and generates deterministic signatures.

### 48% — portable save states

- Added a versioned fixed-size save-state record with level id, flags, scene signature, camera, viewport, frame number, and checksum.
- Supports memory and file round trips and rejects corrupted states.

### 49% — integrated software frontend

- Combines scene reconstruction, camera stepping, BG composition, stream-map fallback, OAM rendering, and display brightness.
- Adds a movable diagnostic OAM marker while keeping it explicitly separate from original actor behavior.

### 50% — PC window and whole-cartridge frontend validation

- Added an optional X11 window backend with keyboard input and approximately 60 Hz presentation.
- Added headless frontend preview and whole-cartridge validation tools.
- Local validation initialized, advanced, and rendered 230/230 scenes with zero failures.
- Supported-ROM frontend signature: `2BA007DBD5D4A725`.
- Expanded configured validation from 46 to 53 tests; all seven newly introduced tests pass locally with warnings treated as errors.

## Accuracy boundary

The OAM representation, OBJ tile decoding, size pairs, priority model, window logic, color arithmetic, input/replay/save formats, and host presentation are functional. The current marker and animation timeline are host validation systems; they are not claimed to be translations of Donkey Kong's original player callback or animation scripts.

## Next measurable targets

- Trace and translate an original player/object callback that creates OAM entries.
- Identify original animation-script records and bind them to the OAM runtime.
- Complete neighboring-cell/material collision side effects of `$81:800D`.
- Implement dynamic map streaming while the camera moves.
- Add exact main/subscreen per-pixel composition and remaining color-window controls.
- Begin SPC700/audio command reconstruction, menus, progression, and SRAM compatibility.
