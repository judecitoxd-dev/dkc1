# dk1 — clean-room PC reimplementation workspace

This repository is an early clean-room native PC reimplementation workspace for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 26%**

The percentage measures completed, tested reimplementation work. It is not the percentage of ROM bytes copied into C. ROM bytes, original graphics, music, level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 55% |
| Semantic portable C | 43% |
| PC rendering / widescreen | 45% |
| Input / saves / menus / compatibility | 25% |
| Audio | 0% |

The runtime now resolves all 230 level/location ids to their real terrain profiles, parses all 23 compact PPU presets, parses the 30 VRAM package lists and their 96 DMA records, translates the `$B8:982F` graphics decompressor, reconstructs ROM- and WRAM-sourced VRAM uploads, and can render authentic decompressed 4bpp cartridge tiles with a local-ROM palette.

The executable frame path remains functional. `dk1_asset_probe` previews raw ranges, `dk1_level_probe` renders real map/collision metadata, and `dk1_vram_probe` reconstructs a selected VRAM package and produces a tile sheet. A complete original-compatible level is still not playable.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/FRAME_RUNTIME.md`](docs/FRAME_RUNTIME.md), [`docs/ROM_ASSETS_TERRAIN.md`](docs/ROM_ASSETS_TERRAIN.md), [`docs/LEVEL_MAP_PIPELINE.md`](docs/LEVEL_MAP_PIPELINE.md), and [`docs/VRAM_PACKAGE_PIPELINE.md`](docs/VRAM_PACKAGE_PIPELINE.md).

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

There are currently **33 automated tests**: 32 C runtime tests and one Python control-flow test.

## Level map and collision preview

```bash
./build/dk1_level_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 8 level0-diagnostic.ppm
```

## Reconstructed VRAM tile sheet

```bash
./build/dk1_vram_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  1 0xB9A1DC 0x3000 0 256 package1.ppm
```

This applies package 1, reads palette data from the local ROM, and renders 256 reconstructed 4bpp tiles beginning at VRAM word `$3000`.

## Will this become functional?

The architecture supports a functional native port and now reaches original map, collision, PPU, DMA-package, compressed graphics, and palette data. A playable original-compatible build still needs exact level initialization sequencing, palette-upload aggregation, full background composition, material/neighbor collision side effects, translated actor callbacks and animation, a PC window backend, audio, saves, menus, and frame-trace comparison against the SNES version.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every translated routine keeps a reference to its SNES source address and behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering preserves simulation coordinates and changes only the host viewport.
