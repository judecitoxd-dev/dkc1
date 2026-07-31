# Player dispatcher, terrain contacts, streaming, and APU mailbox

## Player object entry

The primary player objects use type records 1 and 2. Their callbacks are `$BF:84AB` and `$BF:84A4`. The entry difference is the player-array offset written to direct-page `$84`; both continue at `$BF:84AD`.

`$1029` is doubled and used to index the 87-word jump table at `$BF:84CA`. Portable code retains the original bank-`$BF` source address for every state.

## Shared routines translated

- `$BF:8607-$BF:8645`: velocity magnitude to camera lead, including inversion through `$0C69 & $4000`.
- `$BF:8689-$BF:86E8`: partner-type-dependent value at `$1139`.
- `$BF:86E9-$BF:8717`: left/right level-bound velocity cancellation.
- `$BF:8778-$BF:87FA`: external event transitions.
- `$BF:8756` and `$BF:87FD`: ordered wrapper plans for states 0 and 1.

The individual routines called by those wrappers are not silently treated as translated.

## Terrain contacts

The contact helper queries left, center, and right foot positions through either an in-memory terrain layout or the ROM-backed terrain view. It aggregates nearest floor and shape/material flags. Exact player collision response remains inside untranslated state handlers.

## Dynamic streaming

The host tracker uses the confirmed 32-pixel map unit. It detects visible columns entering and leaving as the camera moves and is connected to the software frontend. It does not yet reproduce every original VRAM DMA destination or timing detail.

## APU mailbox

The audio boot routine at `$CA:B0EE` waits for IPL signature `$BBAA`, writes upload address `$04B8` to APUIO2/3, writes command `$01CC` to APUIO0/1, and uploads 40 indexed bytes.

The runtime command path at `$CA:B1AF` waits for APUIO0 to equal the current token, writes the command to APUIO1, toggles bit 7, forces bit 0, and writes the new token to APUIO0.

The portable mailbox models transport only. It does not execute the SPC700 program or synthesize audio yet.
