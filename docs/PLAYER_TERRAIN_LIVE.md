# ROM terrain and live player dispatcher bridge

## Coordinate model

The original horizontal coordinate is a 16-bit world value. Treating it as signed broke level regions above `$8000`, even though subtracting two wrapped 16-bit values still produces the correct nearby screen delta. `Dk1PlayerMotion.world_x` is therefore unsigned, while horizontal velocity remains signed 8.8.

Terrain Y uses the collision map's bottom-origin 0–511 space. When ROM terrain is active, the preview uses:

```text
screen_y = 512 - camera_y - world_y
```

The ROM-less fallback retains its earlier 224-pixel diagnostic origin.

## Terrain support

`player_terrain_runtime` reads the scene's `Dk1LevelTerrainConfig`, shape flags at `$81:8409` and collision descriptors selected by the 32-pixel column map. It probes seven pixels left/right of the player center and the center itself.

Initial placement scans the 16 collision rows and selects the surface closest to the requested preview position. During movement, the resolver accepts a crossed surface while descending or a nearby surface while grounded. The player center is maintained 16 pixels above the selected floor.

Descriptor and shape attributes are retained in `floor_attributes`. The bridge does not yet invent behavior for water, damaging, conveyor or other material bits whose complete original side effects remain untranslated.

## Live state path

`player_live_runtime` associates every preview frame with the original dispatcher table:

```text
grounded → state 1 / handler $BF:87FD plan
jump     → state 11 / handler $BF:8FA7 local semantics
landing  → state 1
```

State 11 is executed through `dk1_player_state_step_compact`. Its required `MOVE` call is satisfied by the translated fixed-point integrators and the ROM terrain resolver. This is the first frontend path that executes a translated player-state handler every airborne frame rather than merely displaying dispatcher metadata.

It is not yet the entire original state machine. Grounded input remains a host bridge around the state-1 call plan, and wall, ceiling, linked-object and material-specific transitions are pending.
