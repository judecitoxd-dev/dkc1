# dk1 — clean-room PC reimplementation workspace

This repository is an early clean-room native PC reimplementation workspace for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 11%**

The percentage measures completed, tested reimplementation work. It is not the percentage of ROM bytes copied into C. ROM bytes, original graphics, music, level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 42% |
| Semantic portable C | 18% |
| PC rendering / widescreen | 15% |
| Input / saves / menus / compatibility | 10% |
| Audio | 0% |

Current verified analysis covers **725 routine entries**, **21,143 unique instruction addresses**, and **423 confirmed indirect edges**. The C runtime now models reset, memory clearing, early PPU setup, NMI scheduling, state sequencing, all 230 level dispatch records, both object pools, 122 object types, the original object selection policy, camera clamping, three confirmed parallax profiles, SNES controller state, host callback registration, translated object invocation, a camera-relative render queue, wrapped 8x8 tilemap drawing, and an integrated native frame step.

The executable now runs a deterministic host frame through input, object scheduling, a translated C callback, PPU scroll calculation, and visible-object generation. It still does **not** load an original level or provide complete gameplay.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/FRAME_RUNTIME.md`](docs/FRAME_RUNTIME.md), [`docs/OBJECT_CAMERA.md`](docs/OBJECT_CAMERA.md), and [`docs/OBJECT_SCHEDULER.md`](docs/OBJECT_SCHEDULER.md).

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

There are currently **18 automated tests**: 17 C runtime tests and one Python control-flow test.

## Will this become functional?

The architecture now supports a functional native port: a frame can read input, select and invoke translated object callbacks, calculate layer scroll, and generate host rendering work. Completion is still not automatic or guaranteed. A playable original-compatible build needs collision and terrain queries, translated gameplay callbacks, real level/tilemap and asset decoding, sprite animation, audio, saves, menus, and trace comparison against the SNES version.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every translated routine keeps a reference to its SNES source address and behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering preserves simulation coordinates and changes only the host viewport.
