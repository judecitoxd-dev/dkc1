# Scene reconstruction pipeline

No ROM, decoded assets, palettes, VRAM dumps, or generated screenshots are committed. Every scene is reconstructed from the user's local USA Rev. 2 cartridge dump.

## Scene recipes

The 230 level/location ids use 31 unique bank-`$B9` initialization callbacks. Each translated recipe records:

- Initializer callback identity.
- One of the 23 PPU presets.
- One of the 14 terrain/streaming profiles.
- Primary VRAM package.
- Palette upload source, CGRAM destination, and transfer count.
- The special inverted-palette path used by callback `$87B7`.

## Package sequencing

A scene applies its primary package followed by the shared packages used by the initializer. The portable plan preserves the alternate package ids used by special states and level ids `$006C`, `$00D2`, and `$00D3`.

## CGRAM

The translated palette operation reproduces the bank-`$B9` transfer count: X units correspond to `X * 8` bytes or `X * 4` colors. Scene memory owns all 256 CGRAM colors and can convert them to RGBA without requiring a manually supplied palette address.

## Camera bounds

The table beginning at `$BC:8000` selects one min/max record for every level id. These bounds are applied independently of host viewport width, so the same simulation camera can drive a 256-pixel SNES view or a wider PC view.

## PPU layouts and rendering

The PPU layout model derives screen base, character base, tilemap dimensions, layer enable state, tile size, and 2/4/8bpp depth from the reconstructed register preset. Renderers support tile flips, palette selection, transparent color zero, tilemap screen wrapping, and camera scrolling.

`dk1_scene_probe ROM LEVEL X Y WIDTH HEIGHT OUTPUT.ppm` reconstructs scene VRAM/CGRAM/PPU state and emits a host-sized preview. When visual metatile definitions are directly readable from the ROM it renders the corrected `$81:8CEF` streaming map. Other scenes fall back to their reconstructed VRAM tilemaps instead of failing.

The stream-map address is `row * 128 + column * 2`. This is intentionally different from the collision query's column-oriented/reversed-Y access. Map cell zero is the blank visual sentinel and performs no metatile read.

## Whole-cartridge validation

`dk1_scene_validate ROM` reconstructs all 230 level ids and hashes their recipe, PPU state, terrain configuration, camera bounds, VRAM, and CGRAM. Local validation of the supported dump produced:

```text
scenes=230 failed=0 signature=6F88519C029414AB
```

All 31 unique scene recipes also produced a preview locally. Twenty-three used direct streaming-map composition and eight used the VRAM fallback.

## Accuracy boundary

The memory bootstrap, table parsing, package application, palette uploads, camera bounds, and tile decoding are source-addressed clean-room translations. The current compositor is diagnostic: it does not yet reproduce all SNES priority ordering, windows, color math, animated streaming updates, sprites, or every initializer side effect. A rendered preview is evidence that the scene data path is connected, not a claim of pixel-perfect gameplay.
