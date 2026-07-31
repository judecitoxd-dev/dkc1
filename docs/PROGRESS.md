# Progress accounting

## Current total: 64%

Sixty-four percent refers to completed engineering infrastructure and translated systems, not sixty-four percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 73% | 10.95% |
| Portable gameplay/system C | 45% | 81% | 36.45% |
| Graphics, camera, tilemaps, widescreen | 15% | 87% | 13.05% |
| Audio | 8% | 22% | 1.76% |
| Input, saves, menus, compatibility | 7% | 64% | 4.48% |
| Validation and packaging | 5% | 72% | 3.60% |

Weighted pipeline foundation: **75.29%**. The public headline remains **64%** because no original-compatible level is playable from start to finish.

## Completed in the 58-to-64% stage

### 59% — exact player input fan-out

- Translated the action order at `$BF:B2C5-$BF:B3D2`.
- Preserves directional precedence, neutral handlers, held-button branches and the `$1917` B-release side effect.
- Exposes the ordered calls without pretending the individual action handlers are already translated.

### 60% — compact player states 6–19

- Added executable local semantics for 13 compact handlers.
- Covers target following, input wrappers, scripted launch/timer behavior, gravity clamping, no-op/movement states, countdown transitions and collision-stop wrappers.
- Unknown callees are returned as explicit required-call flags.

### 61% — player interrupt guard

- Translated local side effects of `$BF:A132-$BF:A1B8`.
- Handles requests `$01`, `$20`, `$40` and `$80`, preserving state transitions `$19`, `$15`, `$01`, `$1A`, effect ids and abort behavior.

### 62% — translation coverage and ROM validation

- Tracks untranslated, plan-only and local-semantic state levels.
- Current coverage is 13 executable local states plus two exact wrapper plans out of 87.
- Supported-ROM translation signature: `0197E755C75F01D8`.

### 63% — dynamic tilemap VRAM writes

- Converts entering 32-pixel map columns into four 8-pixel tile columns.
- Writes authentic metatile words into 32x32, 64x32, 32x64 or 64x64 reconstructed BG tilemaps with ring wrapping.
- This binds camera column tracking to real VRAM state; exact SNES DMA scheduling remains pending.

### 64% — exact SPC bootstrap image

- Reads the 40-byte upload source used by `$CA:B10E` from `$8A:A342`.
- Supported-ROM bootstrap signature: `F2BE1E6916EC4EC2`.
- The CPU transport and boot payload are now identified; SPC700 execution and DSP synthesis remain pending.

## Validation

- Six new C tests pass locally with `-Wall -Wextra -Wpedantic -Werror`.
- Configured validation increases from 60 to 66 tests.
- `dk1_gameplay_validate` reports `planned=15`, `local=13`, `invalid=0`.
- Existing frontend validation remains 230/230 scenes with zero failures.

## Next measurable targets

- Translate movement-heavy player handlers and their velocity approach tables.
- Decode original animation script records and produce player OAM entries.
- Connect dynamic tile writes to exact NMI/DMA timing.
- Load and execute enough SPC700 code for the first sound command.
- Translate a common enemy/object behavior against the existing scheduler.
