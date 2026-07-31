# Terrain collision envelope and scheduler-driven animated actor

## Solid-point terrain queries

`dk1_rom_terrain_point_solid` reuses the confirmed 32-pixel collision-cell decoder and the translated 64 shape curves. A point is solid when it lies inside the cell and at or below that shape's surface in the game's bottom-origin 0–511 collision space.

This does not create a second collision format. The returned `Dk1TerrainSample` preserves the source block index, shape ID, local X and combined descriptor/shape attributes.

## Player body envelope

`player_terrain_runtime` now resolves three contact groups:

- **Feet:** the existing left/center/right support probes follow slopes and resolve landing.
- **Leading side:** three points between foot and head block horizontal integration and clear horizontal velocity/subpixels.
- **Head:** three points across the top stop upward motion when the player enters a solid cell underside.

Wall and ceiling attributes are stored separately. The runtime also reports left/right blocking, ceiling hits, contact counters and support-material changes. It deliberately does not invent water, damage, conveyor or other material effects whose original handlers remain unresolved.

## Original animated-render callback path

Object type `$73` (decimal 115) maps to callback `$BF:8453`. The callback consists of the original animation interpreter call followed by the common object renderer.

`object_actor_runtime` supplies a measurable portable path for that callback:

```text
object slot/type table
→ original scheduler pass
→ callback $BF:8453 verification
→ animation script interpreter
→ frame definition
→ screen OAM
→ frame graphics DMA
→ private VRAM
```

The test initializes animation 0 because its first frame and layout are already independently verified (`$0330`, 12 pieces, 576 DMA bytes). The optional touch-deactivate behavior is explicitly a clean-room host interaction policy; it is not presented as the original gameplay identity or behavior of object type `$73`.
