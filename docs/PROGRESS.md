# Progress accounting

## Current total: 70%

Seventy percent refers to completed engineering infrastructure and translated systems, not seventy percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 76% | 11.40% |
| Portable gameplay/system C | 45% | 87% | 39.15% |
| Graphics, camera, tilemaps, widescreen | 15% | 89% | 13.35% |
| Audio | 8% | 35% | 2.80% |
| Input, saves, menus, compatibility | 7% | 65% | 4.55% |
| Validation and packaging | 5% | 76% | 3.80% |

Weighted pipeline foundation: **80.05%**. The public headline remains **70%** because no original-compatible level is playable from start to finish.

## Completed in the 64-to-70% stage

### 65% — player states 20–26

- Added an exact state-20 call plan without pretending its linked-object helper is complete.
- Added executable semantics for states 21–26.
- Preserves guard exits, input modes `$0C`, `$00`, `$02`, `$07`, animation dispatch and ordered shared calls.

### 66% — player states 27–34

- Added executable semantics for scripted movement setup, camera-relative launch, animation-preserving movement, facing-dependent velocity and clamped vertical acceleration.
- State transitions to `$1D`, `$1E` and `$20` and writes to `$051A`, `$11A1`, `$0E89`, `$0EF1` and `$0F25` are represented directly.

### 67% — original animation frame records

- Translated the normal-frame timing path beginning at `$BE:80E1`.
- Uses `$1105` accumulator, `$1139` speed, `$116D` script pointer and `$0D11` frame output.
- Reads animation roots from the table at `$BE:8572`.

### 68% — common animation commands

- Supports restart commands `$80/$91`, long/local external calls `$81/$83`, script jump `$82`, callback installation `$84` and event commands `$8E-$90`.
- Complex paired-object commands `$85-$8D` remain explicit unsupported results.
- Supported-ROM animation zero produces first frame `$0330`.

### 69% — SPC700 bootstrap CPU subset

- Added the exact instruction subset used by the 40-byte payload at `$04B8`.
- Implements MOV, CMP, AND, conditional/unconditional branches, indirect store and indexed indirect jump operations required by the payload.

### 70% — executable SPC transfer and launch

- Executes the cartridge bootstrap itself to transfer bytes through `$F4-$F7`.
- Verifies alternating odd upload tokens and an even launch token.
- Follows `JMP [!$00F6+X]` into the uploaded payload.
- Supported validation performs two byte transfers and reaches `$2000` after 40 executed instructions.

## Validation

- Three new focused C tests pass with `-Wall -Wextra -Wpedantic -Werror`.
- Focused local workspace passes 36/36 tests.
- Configured project validation increases from 66 to 69 tests.
- `dk1_gameplay_validate` reports 30 planned states, 27 local states and zero invalid translated handlers.

## Next measurable targets

- Translate the movement-heavy player states beginning at 35 and their fixed-point integration.
- Implement paired-object animation commands and bind real frames to OAM pieces.
- Trace the larger SPC700 driver upload and execute the first real sound command.
- Connect dynamic tile writes to exact NMI/DMA queues.
- Translate one common enemy/object type through scheduler, collision and OAM.
