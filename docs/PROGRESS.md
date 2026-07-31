# Progress accounting

## Current total: 77%

Seventy-seven percent refers to completed engineering infrastructure and translated systems, not seventy-seven percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 80% | 12.00% |
| Portable gameplay/system C | 45% | 93% | 41.85% |
| Graphics, camera, tilemaps, widescreen | 15% | 90% | 13.50% |
| Audio | 8% | 48% | 3.84% |
| Input, saves, menus, compatibility | 7% | 65% | 4.55% |
| Validation and packaging | 5% | 80% | 4.00% |

Weighted pipeline foundation: **84.74%**. The public headline remains **77%** because no original-compatible level is playable from start to finish.

## Completed in the 70-to-77% stage

### 71% — player states 35–39

- Added exact local semantics for fixed-point scripted movement, linked-object frame clearing and the type-$30/$31 linked-player wrappers.
- State 35 reproduces velocity/acceleration integration, arithmetic division by 32, source-relative positioning, clamped vertical term and subpixel branch updates.
- States 37–39 retain every unresolved helper as an explicit required-call flag.

### 72% — player states 40–44

- Added mode-dependent timers, input routing, partner branches, initialization/reset state and camera-relative exit behavior.
- Translated the local gravity helper at `$BF:97C8`, including its `$145D` countdown behavior and `-$0300` vertical clamp.

### 73% — player states 45–49

- Added the scripted vertical launch state, collision/input wrapper, query-driven transition to state `$4C` and off-camera paired-player reset path.
- Preserves writes to `$0579`, `$0C69`, `$1029`, `$11D5`, `$123D`, `$1631`, `$1929`, `$051A` and player/partner velocity fields.

### 74% — reverse animation playback

- Translated the negative-speed path beginning at `$BE:8414`.
- Walks records backwards, follows reverse pointer records and preserves the original accumulator and script-pointer movement.

### 75% — linked-object animation commands

- Implemented commands `$85-$8D` from `$BE:81E2-$BE:83CA`.
- Supports primary `$0512`, secondary `$16F5` and direct `$1375` links.
- Applies paired frames, `$13E9/$145D` motion values and linked animation-speed clearing exactly where the original handlers do.
- Missing required links return an explicit `LINK_REQUIRED` result; command `$8D` remains a no-op when its direct link is zero.

### 76% — audio source catalog

- Parsed the exact 27 three-byte source pointers at `$8A:B15E-$8A:B1AE`.
- Supported-ROM catalog signature: `D66EA2B33D4342BD`.

### 77% — SPC upload-block loading

- Translated the four-byte block header and byte transfer performed by `$8A:B4EB-$8A:B538`.
- Every block exposes source, payload address, length and SPC target.
- All 27 blocks target `$12A0`, load successfully into a 64 KiB SPC RAM image and contain 65,195 payload bytes in total.
- Supported-ROM payload signature: `6CC614324FAED0E8`.

## Validation

- Added tests for player states 35–49 and the complete audio catalog/loader.
- Expanded the animation test with commands `$85`, `$8A`, `$8D` and reverse playback.
- Focused local workspace passes 38/38 tests with `-Wall -Wextra -Wpedantic -Werror`.
- Configured project validation increases from 69 to 71 tests.
- `dk1_gameplay_validate` reports 45 planned states, 42 local states and zero invalid translated handlers.

## Next measurable targets

- Translate player states 50 onward and the shared movement/collision helpers they depend on.
- Identify the original frame-to-OAM-piece tables and render Donkey Kong instead of the diagnostic marker.
- Execute one of the 27 loaded SPC programs far enough to process its first command.
- Connect dynamic tile writes to the exact NMI/DMA queue.
- Translate one common enemy or barrel behavior through scheduler, collision, animation and OAM.
