# dk1 — clean-room PC reimplementation workspace

This repository is a clean-room native PC reimplementation workspace for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 40%**

The percentage measures completed, tested reimplementation work. It is not the percentage of ROM bytes copied into C, and it is not the percentage of gameplay that is currently playable. ROM bytes, original graphics, music, level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 62% |
| Semantic portable C | 55% |
| PC rendering / widescreen | 68% |
| Input / saves / menus / compatibility | 30% |
| Audio | 0% |

The runtime now reconstructs a scene from any of the 230 level/location ids: it selects one of 31 initializer recipes, applies the correct PPU preset, terrain profile, VRAM package sequence, CGRAM uploads, and exact camera bounds, then exposes VRAM/CGRAM/PPU state to 2/4/8bpp host renderers. It also preserves the corrected visual-streaming map order used by `$81:8CEF` and treats map cell zero as the blank sentinel.

Local whole-cartridge validation reconstructed **230/230 scenes with zero failures** and produced the deterministic signature `6F88519C029414AB`. All 31 unique scene recipes produced previews: 23 through direct streaming-map composition and eight through reconstructed VRAM fallback.

This is a major functional data-path milestone, but it is still **not a playable or pixel-perfect port**. The compositor does not yet reproduce every SNES priority, window, color-math, animation, sprite, and initializer side effect.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/SCENE_RECONSTRUCTION.md`](docs/SCENE_RECONSTRUCTION.md), [`docs/FRAME_RUNTIME.md`](docs/FRAME_RUNTIME.md), [`docs/LEVEL_MAP_PIPELINE.md`](docs/LEVEL_MAP_PIPELINE.md), and [`docs/VRAM_PACKAGE_PIPELINE.md`](docs/VRAM_PACKAGE_PIPELINE.md).

## ROM required locally

No ROM is included. Put your legally obtained ROM at:

```text
rom/Donkey Kong Country (USA) (Rev 2).sfc
```

Expected SHA-256:

```text
628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527
```

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/dk1_pc
```

There are currently **46 automated tests configured**: 45 C runtime tests and one Python control-flow test. The 13 tests introduced in the 26-to-40% stage pass locally with warnings treated as errors.

## Scene preview

```bash
./build/dk1_scene_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 0 128 384 224 scene0.ppm
```

Arguments after the ROM are level id, camera X, camera Y, viewport width, viewport height, and output path. The viewport can be wider than the SNES display while the original camera bounds remain active.

## Validate every scene

```bash
./build/dk1_scene_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

Expected result for the supported dump:

```text
scenes=230 failed=0 signature=6F88519C029414AB
```

## Other probes

```bash
./build/dk1_level_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 8 level0-diagnostic.ppm

./build/dk1_vram_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  1 0xB9A1DC 0x3000 0 256 package1.ppm
```

## What remains before gameplay

A playable original-compatible build still needs translated player and enemy callbacks, exact movement and neighboring-cell collision side effects, sprite/OAM animation, dynamic level streaming, complete priority/window/color-math behavior, a real PC window and controller backend, audio, saves, menus, and frame-trace comparison against the SNES version.

## Project rules

- Never commit a ROM, extracted assets, decompressed dumps, or generated screenshots.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every translated routine keeps a reference to its SNES source address and behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering preserves simulation coordinates and changes only the host viewport.
