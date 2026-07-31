# Verified ROM notes

The user-provided dump was inspected locally; the ROM itself is not stored here.

- Size: `0x400000` bytes (4 MiB)
- Mapping: HiROM
- Internal title: `DONKEY KONG COUNTRY`
- Map mode: `0x31`
- Cartridge type: `0x02`
- ROM size code: `0x0C`
- SRAM size code: `0x01`
- Region code: `0x01`
- Internal version: `2`
- Emulation reset vector: `$8000`
- Native NMI vector: `$A976`
- Native IRQ vector: `$A99E`
- SHA-256: `628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527`

## Confirmed systems represented in portable C

- `$00:A8E6` and `$00:A920`: full 128 KiB WRAM clear.
- `$00:A8C2`: 64 KiB VRAM clear.
- `$80:A97A`: NMI callback dispatch through `$001C`.
- `$80:80A9`: main per-frame scheduler callback.
- `$0508/$050A/$050C`: current state callback, sequence table, and index.
- `$80:9723`: level frame dispatch for 230 level/location identifiers.
- `$BF:8000-$BF:80F4`: primary object scheduling policy.
- `$BF:815E-$BF:8175`: secondary object pool update.
- `$BF:817C`: 122 object-type callback/attribute records.
- `$088B/$0895`: camera X/Y origin.
- `$1B23/$1B25`: horizontal camera limits.

## Controller input confirmed

`$00:C180-$00:C1AF` reads `$4218/$421A` and stores:

- `$0500/$0502`: held masks for controllers 1 and 2.
- `$0504/$0506`: newly pressed masks using `(new ^ old) & new`.

`$00:C1B2-$00:C20A` selects the active held/pressed values at `$050E/$0510` and merges both controllers in shared-input modes.

## Scroll profiles confirmed

- `$80:8973-$80:89B3`: BG1 follows camera; BG2 uses half X/Y speed.
- `$80:8CA0-$80:8CEA`: BG2 uses half X and quarter Y speed; BG3 Y comes from level state.
- `$80:8DF0-$80:8E42`: BG2 X uses half speed and BG3 X uses 1.5x camera speed.

## Host runtime boundary

SNES callback addresses remain source identities in a host registry. Translated C callbacks receive frame and object context. Untranslated callbacks remain visible as missing rather than being silently emulated or guessed. The host render path projects object coordinates relative to camera, orders by the confirmed priority bits, and can draw wrapped synthetic/indexed tilemaps at widescreen dimensions.

No original ROM bytes, graphics, music, or level geometry are committed.
