# Progress accounting

## Current total: 96%

Ninety-six percent refers to completed engineering infrastructure and translated systems, not ninety-six percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 94% | 14.10% |
| Portable gameplay/system C | 45% | 100% state coverage + core motion | 45.00% |
| Graphics, camera, tilemaps, widescreen | 15% | 99% | 14.85% |
| Audio | 8% | 80% | 6.40% |
| Input, saves, menus, compatibility | 7% | 85% | 5.95% |
| Validation and packaging | 5% | 98% | 4.90% |

Weighted engineering foundation: **96.20%**, rounded to the public **96%** headline. This still does not mean that an original-compatible level is playable from start to finish.

## Completed in the 94-to-96% stage

### 95% — exact shared fixed-point motion and controlled preview

- Translated `$BF:AF81` default/owner gravity and its `-$0800` clamp.
- Translated `$BF:AFB2` light gravity with `-$0140/-$0200` terminal values.
- Translated horizontal `$BF:AFE4` and vertical `$BF:B012` signed 8.8 integration, including fractional carry into the integer coordinate.
- Translated the nine response functions selected at `$BF:B29E` and the local target approach used by `$BF:B1D5`.
- Added `player_preview_runtime`: L/R select `-$0180/$0180`, B jumps, facing flips the authentic OAM frame, and movement is no longer direct screen-pixel addition.
- Landing remains a flat preview baseline. Original ROM terrain/material collision is not claimed complete.

### 96% — clean-room IPL transfer and relaunch bridge

- Added an IPL behavior model that exposes ready ports `$AA/$BB`, accepts command `$CC`, writes sequential token-acknowledged bytes and launches a requested SPC address.
- It operates on the existing 64 KiB SPC RAM image and does not include Nintendo's 64-byte IPL ROM.
- The validator uploads `11 22 33` to `$3000` and relaunches the loaded driver at `$05E8`.
- This advances protocol coverage beyond the `$FFC0` BRK handoff but does not implement IPL instruction timing, DSP or audible output.

## Validation

- Added tests for fixed-point motion, host preview motion and the clean-room IPL protocol.
- Updated the software frontend test for velocity/subpixel movement and jumping.
- Twelve focused tests pass together with `-Wall -Wextra -Wpedantic -Werror`.
- Configured project validation increases from 80 to 84 tests.
- The gameplay validator now reports deterministic motion accumulators and the IPL transfer/relaunch result.

## Next measurable targets

- Connect ROM terrain contacts and material side effects to `player_preview_runtime`, replacing the flat landing plane.
- Advance the actual player dispatcher every frontend frame rather than using a host-only preview controller.
- Translate one common barrel or enemy through scheduler, collision, animation, DMA and OAM.
- Continue from the IPL protocol into the loaded driver's port/timer command loop.
- Add emulator-reference traces and begin menus/progression/SRAM compatibility.
