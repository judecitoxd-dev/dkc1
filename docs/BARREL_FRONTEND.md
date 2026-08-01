# Authentic barrel scene/frontend bridge

## Pipeline

```text
explicit frontend spawn
→ live common-barrel state
→ Y pickup / Y throw debug action
→ fixed-point motion and ROM terrain
→ phase-selected original animation
→ frame pointer table
→ original OAM pieces
→ original frame graphics DMA
→ separate frontend VRAM/OAM render pass
```

## Confirmed normal Barrel frames

| Phase | Animation | First frame | Pieces | DMA bytes |
|---|---:|---:|---:|---:|
| Idle | `$00D2` | `$1BD4` | 7 | 608 |
| Held | `$00D8` | `$1C18` | 6 | 576 |
| Thrown/rolling | `$00DE` | `$1BF8` | 6 | 576 |

The values above are read from the supported Rev 2 ROM's animation and frame
pointer tables. The renderer uses the existing clean-room OAM and NMI-DMA paths.

## Terrain adapter

`barrel_scene_runtime` maps the live barrel callbacks to the existing
`Dk1RomTerrainView`:

- floor crossings use `dk1_rom_terrain_find_floor`;
- wall probes use `dk1_rom_terrain_point_solid`;
- original material attributes are retained by the live barrel runtime.

## Accuracy boundary

The frontend entry point is intentionally explicit. It does not claim that the
barrel was spawned by the original level object list. Y pickup/throw, the
pickup radius and held offset remain portable integration policy. A true 99/100%
claim requires the original object record, scheduler, ownership links and
player carry/throw states to drive this same visual and collision pipeline.
