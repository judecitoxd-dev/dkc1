# dk1 — clean-room PC reimplementation workspace

This repository is an **early clean-room reimplementation workspace** for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 1%**

This percentage measures completion of the reimplementation pipeline, not the percentage of ROM bytes copied into C. ROM bytes, original graphics, music, level data, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / header verification | 100% |
| HiROM bank manifest tooling | 100% |
| Reset-vector and boot-entry analysis | 20% |
| Routine discovery and symbol map | 1% |
| Semantic C reimplementation | 0% |
| PC rendering / widescreen | 0% |
| Audio / input / save compatibility | 0% |

See [`docs/PROGRESS.md`](docs/PROGRESS.md) for the milestone definition.

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
python3 tools/dis65816.py "rom/Donkey Kong Country (USA) (Rev 2).sfc" --pc 00:8000 --count 40
```

## Build the C workspace

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/dk1_pc
```

The current executable is a host-side verification shell. It does not yet run gameplay.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and analysis metadata only.
- Every reimplemented routine must reference its SNES address and describe behavior, not copy an assembly listing verbatim.
- Widescreen work begins after camera, object, and tilemap behavior are represented in portable C.
