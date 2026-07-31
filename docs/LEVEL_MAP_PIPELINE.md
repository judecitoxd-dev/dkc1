# Level map and collision pipeline

No level data, graphics, or generated previews are committed. All reads operate on the user's local USA Rev. 2 ROM.

## Configuration records

`$81:8C67-$81:8CAF` selects one of 14 configuration records. The portable representation exposes:

- 24-bit map pointer.
- Visual metatile bank and 16-bit base.
- 24-bit collision block-half pointer.
- Streaming destination/base value.
- Collision block-count bound.
- 24-bit terrain callback.

The visual bank byte uses the original fallback: zero becomes bank `$80`.

## Two distinct map access patterns

The same scene data is consumed differently by rendering and collision. These layouts must not be conflated.

### Visual streaming

The streaming routine beginning at `$81:8CEF` computes the map address as:

```text
map base + row * 128 + column * 2
```

Portable visual APIs therefore expose `cell(row, column)` using a 64-word row stride. Map cell zero is a blank sentinel and does not read a visual metatile definition.

### Collision query

The terrain path beginning at `$81:800D` uses its own column-oriented calculation and reversed vertical coordinate. The collision module retains that behavior independently of the visual map reader.

## Visual metatiles

A nonzero 32x32 visual map cell expands into a 4x4 group of SNES tilemap words. The clean-room implementation:

- Selects one of four source rows.
- Reverses rows for vertical flip.
- Reverses columns for horizontal flip.
- XORs `$4000`, `$8000`, or `$C000` into tilemap words as the original path does.

The collision bound `$DB` is not applied to visual metatile ids. Real-ROM validation showed visual ids can exceed the collision descriptor count.

Some profiles use visual definition addresses that are not directly readable as cartridge ROM for every cell. `dk1_scene_probe` renders the stream-map path when available and falls back to the reconstructed PPU/VRAM tilemap when it is not.

## ROM-backed collision

The ROM terrain view combines the selected map pointer with the collision block-half pointer. It reads only cells needed for a sample and applies the translated shape profile. No complete level dump is created.

## Diagnostic rendering

`dk1_level_probe ROM CONFIG_ID COLUMNS OUTPUT.ppm` expands actual map metadata and colors tilemap words deterministically. A red line marks the reconstructed floor.

`dk1_scene_probe ROM LEVEL X Y WIDTH HEIGHT OUTPUT.ppm` goes further: it resolves the level initializer, PPU preset, package sequence, CGRAM, camera bounds, and background source before producing a host-sized scene preview.
