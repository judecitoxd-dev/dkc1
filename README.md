# dk1 — clean-room PC reimplementation workspace

This repository is an early clean-room native PC reimplementation workspace for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 16%**

The percentage measures completed, tested reimplementation work. It is not the percentage of ROM bytes copied into C. ROM bytes, original graphics, music, level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 46% |
| Semantic portable C | 27% |
| PC rendering / widescreen | 26% |
| Input / saves / menus / compatibility | 18% |
| Audio | 0% |

Current verified analysis covers **725 routine entries**, **21,143 unique instruction addresses**, and **423 confirmed indirect edges**. The C runtime models reset, PPU setup, NMI scheduling, state sequencing, 230 level dispatch records, both object pools, 122 object types, controller input, host callbacks, object execution, camera and parallax, render queues, wrapped tilemaps, exact Rev. 2 ROM validation, HiROM reads, 4bpp graphics and BGR555 palette decoding, the collision-cell format used by `$81:8000`, all 63 valid slope transforms selected by `$81:84C9`, indexed-to-RGBA presentation, and a deterministic validation actor.

The executable frame path is functional, and `dk1_asset_probe` can read the user's local ROM and create a PPM preview from user-supplied tile and palette addresses. It still does **not** load a complete original level or provide original-compatible gameplay.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/FRAME_RUNTIME.md`](docs/FRAME_RUNTIME.md), [`docs/ROM_ASSETS_TERRAIN.md`](docs/ROM_ASSETS_TERRAIN.md), [`docs/OBJECT_CAMERA.md`](docs/OBJECT_CAMERA.md), and [`docs/OBJECT_SCHEDULER.md`](docs/OBJECT_SCHEDULER.md).

## ROM required locally

No ROM is included. Put your legally obtained ROM at:

```text
rom/Donkey Kong Country (USA) (Rev 2).sfc
```

Expected SHA-256:

```text
628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527
```

## Analyze the ROM

```bash
python3 tools/rom_info.py "rom/Donkey Kong Country (USA) (Rev 2).sfc"
python3 tools/bank_manifest.py "rom/Donkey Kong Country (USA) (Rev 2).sfc" build/banks.json
python3 tools/cfg65816.py \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" build/cfg.json \
  --indirect-map docs/indirect_targets.json \
  --max-instructions 500000
```

The analysis tools export addresses and graph metadata only. Dynamically supplied script and object callbacks remain explicitly unresolved instead of being guessed.

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/dk1_pc
```

There are currently **23 automated tests**: 22 C runtime tests and one Python control-flow test.

## Local asset preview

The probe decodes standard uncompressed SNES 4bpp tiles and BGR555 colors at addresses supplied by the user:

```bash
./build/dk1_asset_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  C00000 C00400 preview.ppm
```

The addresses are hexadecimal 24-bit SNES addresses. Compressed game resources still require their individual decompression formats to be translated.

## Will this become functional?

The architecture supports a functional native port: it can read input, execute translated callbacks, query reconstructed collision cells, calculate layer scroll, decode local ROM graphics, and generate host rendering work. Completion is still not automatic or guaranteed. A playable original-compatible build needs level-table decoding, compressed asset formats, shared player/object collision behavior, translated actor callbacks, sprite animation, audio, saves, menus, and frame-trace comparison against the SNES version.

The preview actor exists only to validate terrain and input integration. It is **not** presented as the original Donkey Kong physics.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every translated routine keeps a reference to its SNES source address and behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering preserves simulation coordinates and changes only the host viewport.
