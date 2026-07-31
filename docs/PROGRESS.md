# Progress accounting

## Current total: 94%

Ninety-four percent refers to completed engineering infrastructure and translated systems, not ninety-four percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 92% | 13.80% |
| Portable gameplay/system C | 45% | 100% state coverage | 45.00% |
| Graphics, camera, tilemaps, widescreen | 15% | 99% | 14.85% |
| Audio | 8% | 70% | 5.60% |
| Input, saves, menus, compatibility | 7% | 70% | 4.90% |
| Validation and packaging | 5% | 95% | 4.75% |

Weighted engineering foundation: **93.90%**, rounded to the public **94%** headline. This still does not mean that an original-compatible level is playable from start to finish.

## Completed in the 90-to-94% stage

### 91% — player states 2–5

- Translated the local branches of `$BF:898F`, `$BF:8ADB`, `$BF:8BDD` and `$BF:8C0E`.
- Preserves partner-state distance thresholds, transitions `$33/$34/$35`, position copying, facing requests, owner floor clamping, countdown behavior, pair launch/reset fields and the state-6 exit.
- External helpers such as `$C486`, `$AF81`, `$AFE4`, `$B012`, `$C121`, `$A067` and `$BC:B882` remain explicit required calls.

### 92% — state 10 and complete dispatcher classification

- Translated the local reset/launch branches beginning at `$BF:8DFA`.
- Preserves speed masking, state `$01/$2B/$53/$56` transitions, link swapping, owner flag clearing, `$0A00/$0080` vertical launches and animation `$05/$60`.
- All 87 dispatcher entries are now classified: 84 local-semantic states and exact plans for 0, 1 and 20.
- Supported-ROM translation signature: `3267767EC866CDAD`.

### 93% — integrated authentic player visual path

- Added `player_visual_runtime`, composing the frame table, piece parser, world/camera transform, OAM writer, frame DMA producer and NMI DMA consumer.
- Frame `$0330` produces 12 OAM pieces and 576 bytes across two DMA records.
- `software_frontend` uses a private frame VRAM image and renders the authentic pieces when a supported ROM and compatible OBJ mode are available.
- The previous marker remains as a safe fallback for ROM-less synthetic tests and incompatible scene configurations.

### 94% — SPC decimal adjust and IPL handoff

- Added SPC700 opcode `$BE` (`DAS A`) with C/H and Z/N effects for the proven startup path.
- Added opcode `$0F` (`BRK`) stack writes and an explicit clean-room handoff to IPL vector `$FFC0`.
- The trace advances from 11 to 13 instructions: `$05E8` startup → `$1076` DAS → BRK → `$FFC0`.
- Nintendo's IPL ROM is not embedded or executed, and this is not yet sound playback.

## Validation

- Added focused tests for the five core states and the complete player visual pipeline.
- Updated coverage and SPC startup tests for the new signatures and IPL handoff.
- Seven focused C tests pass together with `-Wall -Wextra -Wpedantic -Werror`.
- A focused frontend integration build also passes, including authentic visual construction and ROM-less fallback.
- Configured project validation increases from 78 to 80 tests.

## Next measurable targets

- Implement the shared player movement/collision helpers currently represented by required-call masks.
- Initialize and advance the player state runtime inside `software_frontend_step` instead of only moving the preview position.
- Translate a common barrel or enemy through scheduler, collision, animation, DMA and OAM.
- Continue the SPC path after the IPL handoff without embedding proprietary IPL bytes.
- Add emulator-reference traces and begin menus/progression/SRAM compatibility.
