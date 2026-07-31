# Progress accounting

## Current total: 58%

Fifty-eight percent refers to completed engineering infrastructure and translated systems, not fifty-eight percent playable gameplay.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 70% | 10.50% |
| Portable gameplay/system C | 45% | 74% | 33.30% |
| Graphics, camera, tilemaps, widescreen | 15% | 84% | 12.60% |
| Audio | 8% | 15% | 1.20% |
| Input, saves, menus, compatibility | 7% | 62% | 4.34% |
| Validation and packaging | 5% | 68% | 3.40% |

Weighted pipeline foundation: **70.34%**. The public headline remains **58%** because no original-compatible level is playable from start to finish.

## Completed in the 50-to-58% stage

### 51% — original player callback identity

- Confirmed object type 1 enters `$BF:84AB` and type 2 enters `$BF:84A4`.
- Both callbacks select the same state machine after setting the player-array offset.

### 52% — complete player state dispatcher

- Translated the exact 87-word jump table at `$BF:84CA`.
- `$1029` is exposed as the player state and every state resolves to a bank-`$BF` handler.
- ROM validation reports 87 valid entries and signature `2BF70E846ED72122`.

### 53% — common player frame calculations

- Translated horizontal level-bound checks at `$BF:86E9-$BF:8717`.
- Translated velocity-derived camera lead at `$BF:8607-$BF:8645`.
- Translated partner-dependent scale at `$BF:8689-$BF:86E8`.

### 54% — event-driven player transitions

- Translated `$BF:8778-$BF:87FA` event cases 1, 2, 3 and 4.
- Preserves transitions to states `$11`, `$4E` and `$55` and the associated follow-up calls.

### 55% — first state wrapper plans

- Translated the ordered call plans for state 0 (`$BF:8756`) and state 1 (`$BF:87FD`).
- Records abort points, the state-specific argument to `$BF:B2C5`, field clearing and shared callbacks.

### 56% — multi-point terrain contacts

- Added left/center/right floor probes over the previously translated ROM terrain format.
- Aggregates nearest floor and material/shape attributes without copying level data.

### 57% — dynamic host streaming tracker

- Tracks visible 32-pixel columns as the camera crosses map boundaries.
- Reports columns entering/leaving the viewport and increments a deterministic generation counter.
- Connected to the software frontend without changing original camera bounds.

### 58% — SPC700 boot and command mailbox

- Translated the `$CA:B0EE` IPL handshake constants: signature `$BBAA`, upload address `$04B8`, command `$01CC`, and the 40-byte indexed loop.
- Translated the `$CA:B1AF-$CA:B1CA` command-token protocol using APUIO0/APUIO1.
- This establishes CPU↔SPC command transport; SPC program execution and audio synthesis remain pending.

## Validation

- Seven new C tests pass locally with `-Wall -Wextra -Wpedantic -Werror`.
- Configured validation increases from 53 to 60 tests.
- Existing frontend validation still reconstructs and renders 230/230 scenes with zero failures.

## Next measurable targets

- Translate the first movement-heavy player states beyond wrappers 0 and 1.
- Decode original animation script records and write real player OAM entries.
- Reproduce exact terrain neighbor/material side effects.
- Bind dynamic column tracking to actual VRAM tilemap writes.
- Load and interpret the SPC700 program and sound command tables.
