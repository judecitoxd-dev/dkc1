# Progress accounting

## Current total: 98% — held intentionally

This stage again does **not** increase the headline percentage. The barrel is
now visible and controllable through an explicit frontend debug spawn, but it
is not yet created from original level object records or connected to the full
player pickup/throw graph.

## Completed while remaining at 98%

### Authentic barrel scene path

- Added `barrel_scene_runtime` above `barrel_live_runtime`.
- Added ROM-terrain adapters for floor crossings and three-point wall probes.
- Selected original idle/held/thrown animation IDs by barrel type.
- Advanced those scripts through the translated animation interpreter.
- Built their original frame layouts through OAM and frame DMA.
- Confirmed normal Barrel frames `$1BD4`, `$1C18`, `$1BF8`.
- Confirmed 7/6/6 pieces and 608/576/576 DMA bytes.

### Software frontend bridge

- Added `dk1_software_frontend_spawn_barrel`.
- Stored the ROM reference only for the lifetime of the frontend.
- Used Y as a debug pickup/throw action without changing B jump input.
- Stepped live barrel physics and ROM terrain alongside the player.
- Rendered the barrel with a separate OAM/VRAM pass before the player.
- Kept ROM-less frontend tests and fallback behavior unchanged.

## Validation

- Added `barrel_scene_runtime` and `software_frontend_barrel` tests.
- Configured validation increases from 92 to 94 tests: 93 C plus one Python.
- Focused C builds pass with strict warnings; the Rev 2 ROM frame records were
  independently checked for the expected pieces and DMA byte counts.
- The complete repository suite and remote CI were not run in this stage.

## Next measurable targets

- Parse and spawn a common Barrel from an original level object record.
- Connect original player pickup, carry and throw states rather than the Y debug bridge.
- Spawn authentic break/explosion effects and route sound `$14` to the audio path.
- Add enemy collision and a level-completion loop.
- Continue SPC command/timer execution and DSP output.
