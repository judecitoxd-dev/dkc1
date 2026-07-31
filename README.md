# dk1 — clean-room PC reimplementation workspace

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is built from disassembly and behavioral analysis, so it is decompilation-like engineering, but it is **not** Rare's original source code recovered line for line.

## Current status

**Overall engineering progress: 50%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable, and it is not based on how many ROM bytes were converted. ROM bytes, original graphics, music, level geometry, and other copyrighted assets are deliberately not committed.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 65% |
| Semantic portable C | 65% |
| PC rendering / widescreen | 82% |
| Input / saves / menus / compatibility | 60% |
| Audio | 0% |

The runtime reconstructs all 230 level/location scenes from the local ROM and now adds a host-facing frame layer: SNES OAM decoding/encoding, 128-sprite OBJ rendering, all OBSEL size pairs, tile-table selection, sprite animation timelines, mode-1 priority ranking, two-window boolean masks, BGR555 add/subtract/half color math, display brightness, keyboard mapping, deterministic input replay, checksummed portable save states, and a software frontend that combines reconstructed backgrounds with an OAM layer.

Whole-cartridge frontend validation initialized, advanced, and rendered **230/230 scenes with zero failures**. Aggregate frontend signature for the supported ROM: `2BA007DBD5D4A725`.

An optional X11 executable provides a real Linux PC window and keyboard input when X11 development libraries are available. This is a functional host preview path, but it is still **not a complete playable or pixel-perfect port**: the visible marker is diagnostic, not the translated original Donkey Kong actor.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/FRONTEND_RUNTIME.md`](docs/FRONTEND_RUNTIME.md), [`docs/SCENE_RECONSTRUCTION.md`](docs/SCENE_RECONSTRUCTION.md), [`docs/FRAME_RUNTIME.md`](docs/FRAME_RUNTIME.md), and [`docs/VRAM_PACKAGE_PIPELINE.md`](docs/VRAM_PACKAGE_PIPELINE.md).

## ROM required locally

No ROM is included. Put your legally obtained ROM at:

```text
rom/Donkey Kong Country (USA) (Rev 2).sfc
```

Expected SHA-256:

```text
628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527
```

## Build and test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

There are now **53 automated tests configured**: 52 C tests and one Python control-flow test. The seven tests introduced in the 40-to-50% stage pass locally with warnings treated as errors.

## Interactive Linux/X11 preview

When CMake finds X11:

```bash
./build/dk1_x11 \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 384 224
```

Controls: arrows or WASD move the camera; Q/E and U/Z move the diagnostic OAM marker; Escape exits.

## Headless frontend preview

```bash
./build/dk1_frontend_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 90 frontend0.ppm
```

## Validate every frontend scene

```bash
./build/dk1_frontend_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

Expected result:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

## Will the final result be functional?

That is the goal, and the architecture is suitable for a functional native port. It cannot be guaranteed in advance: commercial-game clean-room reconstruction can still uncover unknown formats, timing dependencies, and callback behavior. The project will not be called complete until gameplay works and is validated against the SNES version.

## What still blocks gameplay

- Original player and enemy callbacks, physics, animation data, and object-to-OAM construction.
- Exact neighboring-cell/material collision side effects.
- Dynamic level streaming during gameplay.
- Exact per-pixel main/subscreen behavior and every initializer side effect.
- Audio/SPC700, music, sound effects, menus, SRAM save compatibility, and game progression.
- Emulator-reference frame traces and broad gameplay testing.

## Project rules

- Never commit a ROM, extracted assets, decompressed dumps, or generated screenshots.
- Commit human-authored clean-room C and address-only analysis metadata.
- Every translated routine keeps a reference to its SNES source address and behavior.
- Generated C stubs do not count as progress.
- Widescreen rendering preserves simulation coordinates and changes only the host viewport.
