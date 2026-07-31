# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 90%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 88% |
| Semantic portable C | 99% |
| PC rendering / widescreen | 97% |
| Input / saves / menus / compatibility | 66% |
| Audio loading / driver path | 60% |

The runtime reconstructs and renders all 230 level/location scenes. The original 87-entry player dispatcher now has **79 states with executable local semantics** and three additional exact plans. Only states 2–5 and 10 remain untranslated; plans 0, 1 and 20 still depend on shared helpers that are not complete.

The original frame renderer is now connected to world position, camera position, vertical scroll and object flip flags. Frame `$0330` produces its 12 authentic pieces at screen coordinates and can schedule the exact two graphics transfers consumed by the NMI DMA queue: 448 bytes followed by 128 bytes.

The initial full SPC700 driver image is reconstructed in SPC RAM. The startup routine copies 3,451 bytes from `$8A:A36E` to `$04E8` and a 3,622-byte driver-data block from `$C9:2D95` to `$2380`. A bounded driver tracer executes the first 11 instructions from `$05E8`, mirrors APUIO0 into `$04DE`, initializes X/SP and follows the real call to `$1076`. It stops explicitly at unsupported opcode `$BE`; music/DSP output is not yet implemented.

Current supported-ROM validation targets:

```text
player_states=87 planned=82 local=79 invalid=0
translation=86F04E4A511B0A0B
animation=0330 frame_pieces=12 frame_layout=9A2E475D9D1AB40F
screen_oam=12 frame_dma_records=2 frame_dma_bytes=576
spc_driver_code=D046E1817CE18D5C
spc_driver_data=4D8B5E57A7B1359A
spc_driver_ram=856CF336ECC55C5B
spc_driver_steps=11 spc_driver_pc=1076 spc_driver_stop=BE
```

The existing whole-cartridge frontend validation remains:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

This is still not a complete playable or pixel-perfect port. The five largest missing player states, shared collision/movement helpers, gameplay object initialization, enemies, progression and complete SPC700/DSP execution remain major blockers.

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

There are now **78 automated tests configured**: 77 C tests and one Python control-flow test. The five focused checks for this stage pass locally with `-Wall -Wextra -Wpedantic -Werror`.

## Validate translated gameplay, OAM/DMA and SPC startup

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

Player state addresses and local mutations follow confirmed bank-`$BF` handlers. Screen-coordinate and flip formulas follow `$BB:A8B8-$BB:A987` and the four frame-piece renderer paths. Graphics DMA scheduling follows `$BB:A9A3-$BB:AA1F`, while NMI consumption follows `$81:8CB0`. SPC driver placement follows `$CA:B133-$CA:B15D`; the instruction tracer intentionally stops when it reaches an opcode outside its proven subset.

## What still blocks gameplay

- Translate states 2–5 and 10 plus their large shared movement/collision helpers.
- Connect the player state runtime, animation, screen transform, frame DMA and OAM paths in the live frontend.
- Complete neighboring-cell/material collision effects and object spawning.
- Translate common enemies, barrels, collectibles and level-completion logic.
- Extend the SPC700 core beyond the initial driver call and implement DSP voices, BRR playback, music and effects.
- Implement menus, progression and SRAM compatibility.
- Compare native frame/input/audio traces against an emulator reference.
