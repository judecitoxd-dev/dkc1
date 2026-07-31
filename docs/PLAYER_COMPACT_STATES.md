# Player compact states, input router, streaming and SPC bootstrap

This stage converts small but complete pieces around the original player state machine. It does not claim that the remaining movement-heavy handlers are translated.

## Input router

`$BF:B2C5-$BF:B3D2` stores the selected held/newly-pressed masks and fans held input out in a fixed order: vertical, horizontal, Select, A, Y, X, L, R, B and Start. Up wins over Down and Left wins over Right when contradictory bits are present. Releasing B clears bit `$0040` in `$1917`.

## Compact state runtime

Executable local semantics are implemented for states 6, 7, 8, 9 and 11–19. External routines that still need translation are represented by required-call flags instead of being silently skipped. Important exact effects include state 11 vertical acceleration by `-$70` with a floor of `-$600`, state 14 countdown to state `$0A`, state 9 transition to `$54`, and states 18/19 collision-driven horizontal stop.

## Interrupt guard

The early guard at `$BF:A132` consumes request values in `$1595`. Requests `$40` and `$80` select states and effects exactly as in the ROM. Requests `$01/$20` expose the shared follow-up call and abort the current state.

## Dynamic tile stream

The camera tracker can now expand newly visible 32-pixel columns and write their four tile columns into reconstructed PPU tilemap VRAM. Ring addressing follows the selected BG map dimensions. This is correct data movement, but not yet cycle/NMI-accurate DMA scheduling.

## SPC bootstrap

The 40 bytes uploaded by `$CA:B10E-$CA:B121` are read from `$8A:A342`. The supported USA Rev. 2 image hashes to `F2BE1E6916EC4EC2` with FNV-1a 64.
