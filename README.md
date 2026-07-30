# dk1 — clean-room PC reimplementation workspace

This repository is an early clean-room reimplementation workspace for the SNES game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 3%**

The percentage measures completed, tested reimplementation work. It is not the percentage of ROM bytes copied into C. ROM bytes, original graphics, music, level data, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 75% |
| Routine discovery and symbol map | 20% |
| Semantic portable C | 3% |
| PC rendering / widescreen | 1% |
| Audio / input / save compatibility | 0% |

Current verified analysis covers **115 routine entries** and **3,849 unique instruction addresses**. The C runtime models reset state, WRAM/VRAM clearing, early boot control, initial PPU configuration, the NMI frame scheduler, and the first state-sequence dispatcher. It is not yet playable.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/NMI_SCHEDULER.md`](docs/NMI_SCHEDULER.md), and [`docs/BOOT_VIDEO.md`](docs/BOOT_VIDEO.md).

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
python3 tools/cfg65816.py \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" build/cfg.json \
  --indirect 80:A98B=80:A98E \
  --indirect 80:A98B=80:A998 \
  --indirect 80:A98B=80:80A9 \
  --indirect 80:80C5=80:822C \
  --indirect 80:80C5=80:8279 \
  --indirect 80:80C5=80:9723 \
  --indirect 80:80C5=80:9ADA \
  --indirect 80:80C5=80:82D2
```

`cfg65816.py` exports addresses and graph metadata only. It does not export ROM bytes or a complete assembly listing.

## Build the C workspace

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/dk1_pc
```

There are currently seven automated tests.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and analysis metadata only.
- Every reimplemented routine must reference its SNES address and describe behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering begins only after camera, object, and tilemap interfaces are understood.
