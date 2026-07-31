# dk1 — clean-room PC reimplementation workspace

This repository is an early clean-room native PC reimplementation workspace for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 21%**

The percentage measures completed, tested reimplementation work. It is not the percentage of ROM bytes copied into C. ROM bytes, original graphics, music, level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 50% |
| Semantic portable C | 35% |
| PC rendering / widescreen | 36% |
| Input / saves / menus / compatibility | 22% |
| Audio | 0% |

Current verified analysis covers **725 routine entries**, **21,143 unique instruction addresses**, and **423 confirmed indirect edges**. The runtime now reads the 14 terrain/streaming profiles selected by `$81:8C67`, binds actual map and collision data from the local ROM, expands the original 32x32 visual metatiles with SNES flip semantics, expands complete map columns into 8x8 tilemap words, and produces a diagnostic map with reconstructed collision drawn over it.

The executable frame path remains functional. `dk1_asset_probe` previews raw tile/palette ranges and `dk1_level_probe` renders actual level-layout metadata from the user's local ROM. The level probe intentionally uses diagnostic colors for tilemap words because the corresponding compressed graphics and VRAM package formats are not translated yet.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/FRAME_RUNTIME.md`](docs/FRAME_RUNTIME.md), [`docs/ROM_ASSETS_TERRAIN.md`](docs/ROM_ASSETS_TERRAIN.md), and [`docs/LEVEL_MAP_PIPELINE.md`](docs/LEVEL_MAP_PIPELINE.md).

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

There are currently **28 automated tests**: 27 C runtime tests and one Python control-flow test.

## Local asset preview

```bash
./build/dk1_asset_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  C00000 C00400 preview.ppm
```

## Level map and collision preview

The supported ROM contains 14 terrain/streaming profiles. This example renders the first eight 32-pixel columns of profile zero:

```bash
./build/dk1_level_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 8 level0-diagnostic.ppm
```

The output is a structural diagnostic: tilemap entries receive deterministic colors and the reconstructed collision floor is drawn in red. It does not claim to be the final original background image.

## Will this become functional?

The architecture supports a functional native port and now reaches actual level-layout and collision records in the ROM. A playable original-compatible build still needs compressed graphics/package decoding, exact level-to-profile selection, full material/neighbor collision side effects, translated actor callbacks and animation, a real PC window backend, audio, saves, menus, and frame-trace comparison against the SNES version.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every translated routine keeps a reference to its SNES source address and behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering preserves simulation coordinates and changes only the host viewport.
