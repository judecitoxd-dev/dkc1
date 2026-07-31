# Level map and collision pipeline

No level data, graphics, or generated previews are committed. All reads operate on the user's local USA Rev. 2 ROM.

## Configuration records

`$81:8C67-$81:8CAF` selects one of 14 configuration records. The source data is split across several parallel tables, but the portable representation exposes:

- 24-bit column-map pointer.
- Visual metatile bank and 16-bit base.
- 24-bit collision block-half pointer.
- Streaming destination/base value.
- Collision block-count bound.
- 24-bit terrain callback.

The visual bank byte uses the original fallback: zero means bank `$80`.

## Column map

Each column stores 16 words in bottom-to-top order. Portable APIs expose rows in ordinary top-to-bottom order. One map word identifies a 32x32 visual/collision block and includes horizontal/vertical flip flags.

## Visual metatiles

The streaming path beginning at `$81:8CEF` expands a map word into a 4x4 group of SNES tilemap words. The clean-room implementation:

- Selects one of four source rows.
- Reverses rows for vertical flip.
- Reverses columns for horizontal flip.
- XORs `$4000`, `$8000`, or `$C000` into tilemap words as the original path does.

The collision bound `$DB` is not applied to visual metatile ids. Real-ROM validation showed visual ids can exceed the collision descriptor count.

## ROM-backed collision

The ROM terrain view combines the selected map pointer with the collision block-half pointer. It reads only the cells needed for a sample and applies the translated shape profile. No complete level dump is created.

## Diagnostic rendering

`dk1_level_probe ROM CONFIG_ID COLUMNS OUTPUT.ppm` expands actual map columns and colors tilemap words deterministically. A red line marks the reconstructed floor. This confirms pointer binding, map order, metatile expansion, flip semantics, and terrain integration before compressed tile graphics are available.
