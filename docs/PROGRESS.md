# Progress accounting

## Current total: 90%

Ninety percent refers to completed engineering infrastructure and translated systems, not ninety percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 88% | 13.20% |
| Portable gameplay/system C | 45% | 99% | 44.55% |
| Graphics, camera, tilemaps, widescreen | 15% | 97% | 14.55% |
| Audio | 8% | 60% | 4.80% |
| Input, saves, menus, compatibility | 7% | 66% | 4.62% |
| Validation and packaging | 5% | 89% | 4.45% |

Weighted pipeline foundation: **91.17%**. The public headline remains **90%** because no original-compatible level is playable from start to finish.

## Completed in the 84-to-90% stage

### 85% — player states 71–75

- Added local semantics for the `$1A69=$30` state, guarded countdown/reset state, animation/finalization wrapper, `$A3CA` movement wrapper and gravity clamp.
- State 75 reproduces `velocity_y -= $70` with the original `-$0600` floor.

### 86% — player states 76–80

- Added the two state-$02 movement wrappers, collision-target propagation, level-exit state snapshot/restore and the stationary-animation wrapper.
- The collision path doubles horizontal velocity into the selected object, writes request `$01`, installs callback `$884B` and exposes effect `$5F`.

### 87% — player states 81–86 and complete tail coverage

- Added event wrappers, guarded input mode `$07`, the two frame-blink states, linked-object initialization and final animation-only state.
- Current dispatcher coverage is 79 local states plus three exact plans; only states 2–5 and 10 remain untranslated.
- Supported-ROM translation signature: `86F04E4A511B0A0B`.

### 88% — exact world/camera-to-OAM transform

- Connected object world X/Y, camera X, vertical origin/scroll/offset and `$4000/$8000` flip flags to original frame pieces.
- Horizontal/vertical flips use the original size-dependent mirrored coordinate equations for large and small pieces.
- Vertical clipping at `$F0` and 9-bit OAM X wrapping are preserved.

### 89% — original frame graphics DMA producer

- Translated `$BB:A9A3-$BB:AA1F`, which schedules graphics data following each frame descriptor.
- Frame `$0330` starts graphics at `$D7:622A` and schedules two NMI records: 448 bytes to VRAM word `$0400`, then 128 bytes to `$0500` for tile base `$40`.
- The existing NMI consumer can execute these records and clear them.

### 90% — complete startup SPC image and bounded driver entry trace

- Reproduced `$CA:B133-$CA:B15D`: 3,451 driver bytes from `$8A:A36E` to SPC `$04E8` and the `$C9:2D95` block (3,622 bytes) to `$2380`.
- Signatures: code `D046E1817CE18D5C`, data `4D8B5E57A7B1359A`, complete RAM `856CF336ECC55C5B`.
- The bounded SPC subset executes 11 instructions from `$05E8`, mirrors APUIO0 into `$04DE`, initializes X/SP, pushes return `$05FA` and enters `$1076`.
- Execution stops explicitly on unimplemented opcode `$BE`; this is not yet music playback.

## Validation

- Added four new test targets and updated player coverage, raising configured validation from 74 to 78 tests.
- The five focused tests pass together with `-Wall -Wextra -Wpedantic -Werror`.
- The updated gameplay validator checks state coverage, screen OAM count, frame DMA records and SPC startup signatures/trace.

## Next measurable targets

- Translate states 2–5 and 10 with the shared movement/collision helpers they call.
- Wire the newly completed player/OAM/DMA path into a live frontend frame.
- Implement one common enemy or barrel through scheduler, collision, animation and OAM.
- Extend the SPC700 instruction core through the `$1076` initialization routine and first command poll.
- Add reference traces against an emulator for player state, OAM, VRAM DMA and APUIO traffic.
