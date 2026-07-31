# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 84%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 84% |
| Semantic portable C | 97% |
| PC rendering / widescreen | 94% |
| Input / saves / menus / compatibility | 66% |
| Audio loading / bootstrap path | 48% |

The runtime reconstructs and renders all 230 level/location scenes. The original player dispatcher now has **63 states with executable local semantics** and three additional exact plans. States 50–70 cover paired-player frame mirroring, synchronization, scripted swaps, camera exits, timers, reset transitions and late animation wrappers.

The original renderer's frame-pointer table at `$BB:CC9C` is now decoded. Frame `$0330` points to `$D7:620A` and expands into 12 original object pieces: two large and ten small. Their encoded positions, tile stepping and group order can be written into the portable OAM image.

The NMI path now models the exact eight-byte VRAM DMA records consumed by `$81:8CB0-$81:8CE6`, including the active-bank sentinel, channel-0 mode-1 transfer, first-inactive termination and record clearing.

Current gameplay/audio validation for the supported ROM:

```text
player_states=87 planned=66 local=63 invalid=0 translation=81A46995A0BE17BB apu_boot=F2BE1E6916EC4EC2 animation=0330 frame_pieces=12 frame_layout=9A2E475D9D1AB40F spc_steps=40 apu_sources=27 apu_bytes=65195 apu_catalog=D66EA2B33D4342BD apu_payload=6CC614324FAED0E8
```

The whole-cartridge frontend validation remains:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

This is still not a complete playable or pixel-perfect port. Twenty-one player states remain untranslated, the original screen-base preparation and full actor/object OAM pipeline are not yet connected to gameplay, and the loaded SPC700 drivers are not executed by a complete CPU/DSP implementation.

## ROM required locally

No ROM is included. Expected SHA-256:

```text
628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527
```

## Build and test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

There are now **74 automated tests configured**: 73 C tests and one Python control-flow test. The three tests introduced in this stage pass locally with `-Wall -Wextra -Wpedantic -Werror`.

## Validate translated gameplay, frame layout and audio loading

```bash
./build/dk1_gameplay_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

## Interactive preview

When X11 is available:

```bash
./build/dk1_x11 \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 384 224
```

## Accuracy boundary

State addresses and local mutations are tied to confirmed bank-`$BF` routines. Frame pointers, descriptor groups and tile stepping follow the bank-`$BB` object renderer. DMA record fields and clearing follow `$81:8CB0`. External helpers and missing screen-coordinate preparation remain explicit instead of being guessed.

## What still blocks gameplay

- Translate the remaining 21 player states and their largest movement/collision helpers.
- Connect original frame layouts to real player object initialization and screen-coordinate preparation.
- Complete neighboring-cell and material collision side effects.
- Bind dynamic tile updates to the exact producer side of the NMI DMA queue.
- Execute the loaded SPC700 drivers and implement DSP music/effects.
- Translate enemies, barrels, collectibles, menus, progression and SRAM compatibility.
- Compare native frame traces against an emulator reference.
