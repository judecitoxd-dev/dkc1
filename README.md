# dk1 — clean-room PC reimplementation workspace

This repository is an early clean-room reimplementation workspace for the SNES
game identified by the user-provided cartridge dump.

## Current status

**Overall engineering progress: 6%**

The percentage measures completed, tested reimplementation work. It is not the
percentage of ROM bytes copied into C. ROM bytes, original graphics, music,
level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 40% |
| Semantic portable C | 9% |
| PC rendering / widescreen | 6% |
| Audio / input / save compatibility | 0% |

Current verified analysis covers **725 routine entries**, **21,143 unique
instruction addresses**, and **423 confirmed indirect edges**. The C runtime now
models reset state, WRAM/VRAM clearing, early boot control, initial PPU setup,
the NMI scheduler, state sequencing, all 230 level dispatch records, the two
object pools, 122 object-type records, the complete primary/secondary object
selection policy, dynamic object callback composition, the original camera
clamp, and one confirmed BG1/BG2 parallax transform. It is not yet playable.

The camera origin and PPU scroll outputs are separated from host viewport
dimensions. The simulation can preserve original camera and parallax behavior
while a PC renderer requests a wider visible rectangle.

See [`docs/PROGRESS.md`](docs/PROGRESS.md),
[`docs/NMI_SCHEDULER.md`](docs/NMI_SCHEDULER.md),
[`docs/BOOT_VIDEO.md`](docs/BOOT_VIDEO.md),
[`docs/LEVEL_DISPATCH.md`](docs/LEVEL_DISPATCH.md),
[`docs/OBJECT_CAMERA.md`](docs/OBJECT_CAMERA.md), and
[`docs/OBJECT_SCHEDULER.md`](docs/OBJECT_SCHEDULER.md).

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
  --indirect-map docs/indirect_targets.json \
  --max-instructions 500000
```

`cfg65816.py` exports addresses and graph metadata only. It does not export ROM
bytes or a complete assembly listing. Dynamically supplied script and object
callbacks remain explicitly unresolved instead of being guessed.

## Build the C workspace

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/dk1_pc
```

There are currently twelve automated tests.

## Will this become functional?

The architecture is suitable for a functional native PC reimplementation, but
completion is not automatic or guaranteed. The object frame scheduler can now
select the same classes of objects and callback addresses as the original
routine, but the individual callback behaviors still need C translations.
A playable build also requires collision, level/tilemap loading, rendering,
audio, input, saves, menus, and trace validation.

## Project rules

- Never commit a ROM, extracted assets, or generated binary dumps.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every reimplemented routine must reference its SNES address and describe behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering is built around verified camera, object, and tilemap interfaces.
