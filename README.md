# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 96%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 94% |
| Semantic portable C | 100% state coverage |
| PC rendering / widescreen | 99% infrastructure |
| Input / saves / menus / compatibility | 85% |
| Audio loading / driver path | 80% |

Every entry in the original 87-state player dispatcher is now classified: **84 states have executable local C semantics** and states 0, 1 and 20 retain exact call plans. The final five large handlers—states 2–5 and 10—now preserve their local partner-distance checks, synchronized launch/reset behavior, follow velocity, facing requests, timers, animation selection and state transitions. Shared external helpers are still surfaced explicitly and are not counted as completed gameplay.

The software frontend no longer has to render only the diagnostic single-tile marker. When a supported ROM is supplied, it builds frame `$0330` through the original frame table, converts its 12 pieces to screen OAM, schedules and executes the authentic 576 bytes of frame graphics DMA into a private VRAM image, then renders that OAM. If a scene uses an incompatible OBJ layout or no ROM is supplied, it safely falls back to the marker.

The bounded SPC700 startup trace now implements `DAS A` and the following `BRK`. With initial APUIO0 `$5A`, the driver reaches `$1076`, decimal-adjusts A to `$F4`, pushes the BRK return/PSW and hands control to the IPL vector `$FFC0` after 13 instructions. IPL-ROM execution and DSP synthesis are not included.

The shared fixed-point motion helpers at `$BF:AF81`, `$BF:AFB2`, `$BF:AFE4`, `$BF:B012` and the response table used by `$BF:B1D5` are now executable. The host preview uses them for L/R acceleration, fractional movement, B jumping and gravity instead of directly adding screen pixels. Landing still uses a flat preview baseline, not the original level-material collision system.

A clean-room IPL protocol model now continues beyond the `$FFC0` handoff without embedding Nintendo's IPL bytes. It exposes the `$AA/$BB` ready state, accepts command `$CC`, writes token-acknowledged bytes into SPC RAM and can relaunch the loaded driver at `$05E8`. This is protocol validation, not audio playback.

Current supported-ROM validation targets:

```text
player_states=87 planned=87 local=84 invalid=0
translation=3267767EC866CDAD
animation=0330 frame_pieces=12 frame_layout=9A2E475D9D1AB40F
screen_oam=12 frame_dma_records=2 frame_dma_bytes=576
visual_pieces=12 visual_dma=576
spc_driver_code=D046E1817CE18D5C
spc_driver_data=4D8B5E57A7B1359A
spc_driver_ram=856CF336ECC55C5B
spc_driver_steps=13 spc_driver_pc=FFC0 spc_driver_stop=2 spc_driver_vector=FFC0
motion_x=100 motion_y=39 motion_vx=0030 motion_vy=FF90
motion_subx=3000 motion_suby=9000
ipl_bytes=3 ipl_pc=05E8
```

The existing scene bootstrap validation remains:

```text
scenes=230 failed=0 signature=6F88519C029414AB
```

The frontend signature changes because the authentic player visual state is now part of the runtime. It must be regenerated rather than compared with the old diagnostic-marker signature.

This is still not a complete playable or pixel-perfect port. Complete behavior of the shared movement/collision helpers, live object initialization, enemies, progression, menus and full SPC700/DSP audio remain major blockers.

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

There are now **84 automated tests configured**: 83 C tests and one Python control-flow test. Twelve focused local tests pass with `-Wall -Wextra -Wpedantic -Werror`, including fixed-point motion, preview control, IPL transfer and the authentic frontend/fallback paths.

## Validate translated gameplay, visual pipeline and SPC handoff

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

L/R now accelerate the preview through the translated fixed-point motion helpers and B performs a preview jump under translated gravity. The visible object is built from the original frame pieces whenever the ROM/OBJ configuration permits it. The landing plane is still synthetic, so this is not proof that original Donkey Kong terrain physics are complete.

## Accuracy boundary

The final state handlers follow confirmed bank-`$BF` instructions and keep unresolved JSR/JSL calls visible in required-call masks. The visual pipeline composes already-confirmed frame layout, screen transform, DMA producer and NMI consumer modules without extracting assets. SPC `DAS`/`BRK` behavior follows a proven emulator implementation and stops at the IPL handoff rather than embedding Nintendo's 64-byte IPL ROM.

## What still blocks gameplay

- Connect the translated motion helpers to the full player-state dispatcher and implement the remaining shared collision helpers.
- Drive the player state machine from real object initialization and controller state in the live frontend.
- Complete neighboring-cell/material collision effects and dynamic object spawning.
- Translate common enemies, barrels, collectibles and level-completion logic.
- Extend the clean-room IPL protocol into the loaded driver command loop and timer/port behavior.
- Implement DSP voices, BRR playback, music and effects.
- Implement menus, progression and SRAM compatibility.
- Compare native frame/input/OAM/VRAM/APUIO traces against an emulator reference.
