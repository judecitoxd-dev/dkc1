# Progress accounting

## Current total: 84%

Eighty-four percent refers to completed engineering infrastructure and translated systems, not eighty-four percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 84% | 12.60% |
| Portable gameplay/system C | 45% | 97% | 43.65% |
| Graphics, camera, tilemaps, widescreen | 15% | 94% | 14.10% |
| Audio | 8% | 48% | 3.84% |
| Input, saves, menus, compatibility | 7% | 66% | 4.62% |
| Validation and packaging | 5% | 85% | 4.25% |

Weighted pipeline foundation: **88.06%**. The public headline remains **84%** because no original-compatible level is playable from start to finish.

## Completed in the 77-to-84% stage

### 78% — player states 50–56

- Added local semantics for paired-player frame mirroring, position/facing synchronization, fallback animation, shared scripted swaps and the mode `$02`/`$0C` transition wrappers.
- Preserves the frame-range conversion between `$0714-$072B` and `$0DC0-$0DD7`.
- Untranslated helpers remain explicit required-call flags.

### 79% — player states 57–63

- Added timer/effect states, launch and camera-exit transitions, hidden-position toggles, reset wrappers and the guarded input state.
- Preserves transitions to `$3D`, `$3E`, `$42` and `$43` and writes to `$051A`, `$0579`, `$0E89`, `$0EF1`, `$0F25`, `$123D`, `$1375`, `$1929` and paired-player fields.

### 80% — player states 64–70

- Added forward/reverse animation wrappers, timed effect termination, facing synchronization and the reset flow beginning at `$BF:9D5F`.
- State 68 reproduces mode-dependent `$2000/$3000` flags, linked-object propagation, both-player vertical launch and the state `$32` partner path.
- State 70 preserves the `$1811=($1813-2)&$3F` update and transition to state `$28`.

### 81% — original frame-pointer table

- Translated the four-byte entries indexed directly by even frame ids at `$BB:CC9C`.
- Each entry exposes the original bank/address descriptor pointer without committing extracted frame data.
- Frame `$0330` resolves to `$D7:620A`.

### 82% — frame-piece parsing and OAM output

- Translated the descriptor header and interleaved X/Y piece stream consumed by `$BB:AADF`.
- Large pieces use the original two-tile increment and row-wrap rule; small/auxiliary groups use their header tile offsets.
- Frame `$0330` contains two large and ten small pieces.
- Supported-ROM frame-layout signature: `9A2E475D9D1AB40F`.
- Parsed pieces can be emitted into the portable OAM image using the renderer's already transformed base coordinates.

### 83% — exact NMI DMA record model

- Translated the eight-byte queue record beginning at `$170F`: transfer length, VRAM word destination, source address and active/source-bank word.
- Bit 15 marks a record active and the low byte supplies the DMA source bank.
- Processing stops at the first inactive record exactly as in `$81:8CB0`.

### 84% — NMI mode-1 VRAM transfer

- Reproduces channel-0 `$4300=$1801` linear VRAM writes through `$2118/$2119`.
- Clears the active/source-bank word after each completed record.
- Supports VRAM wrapping and deterministic byte/record statistics.
- Exact cycle timing and the producer-side scheduling of every gameplay update remain pending.

## Validation

- Added focused tests for states 50–70, frame `$0330` parsing/OAM output and NMI DMA execution.
- All three new tests pass locally with `-Wall -Wextra -Wpedantic -Werror`.
- Configured project validation increases from 71 to 74 tests.
- `dk1_gameplay_validate` reports 66 planned states, 63 local states, zero invalid translated handlers and the frame-layout signature above.

## Next measurable targets

- Translate player states 71–86 and the shared helpers they expose.
- Feed real player object screen bases and tile attributes into the frame OAM builder.
- Translate the producer that fills `$170F-$17xx` during dynamic map streaming.
- Execute one loaded SPC700 driver far enough to process its first command.
- Translate a common enemy or barrel through scheduler, collision, animation and OAM.
