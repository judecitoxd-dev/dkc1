# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 97%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 96% |
| Semantic portable C | 100% dispatcher coverage + live state bridge |
| PC rendering / widescreen | 99% infrastructure |
| Input / saves / menus / compatibility | 88% |
| Audio loading / driver path | 80% |

Every entry in the original 87-state player dispatcher remains classified: 84 states have executable local C semantics and states 0, 1 and 20 retain exact call plans.

The interactive frontend now uses authentic ROM terrain instead of a flat preview landing line whenever a supported ROM is supplied. It reads the scene's terrain profile, the original 32-pixel collision cells, all 64 shape curves and their material/shape flags. The player is placed on the closest original surface, follows slopes through three foot probes and lands on the crossed surface after a jump.

The player world-X field is now represented as an unsigned 16-bit coordinate. This fixes levels whose camera and collision map begin above `$8000`; the wrapped signed screen delta still matches the original 16-bit object renderer.

The frontend also advances a measurable live dispatcher bridge. Grounded preview frames retain the exact state-1 plan, while jumping enters state 11 at `$BF:8FA7`, executes its `velocity_y -= $70` local semantics with the `-$0600` floor, satisfies its `MOVE` request through the translated 8.8 motion helpers, resolves ROM terrain and returns to state 1 on landing.

The authentic visual path remains connected:

```text
input
→ state 1 plan / state 11 local handler
→ fixed-point movement
→ ROM terrain contacts
→ world/camera transform
→ frame $0330
→ 12 OAM pieces
→ two NMI DMA records / 576 graphics bytes
→ PC render
```

The bounded SPC700 and clean-room IPL work from the previous stage remains: startup reaches `$FFC0`, the public transfer protocol can upload bytes and the loaded driver can be relaunched at `$05E8`. DSP music and effects are not implemented.

Current supported-ROM gameplay validation additionally checks:

```text
terrain_floor=351 terrain_center=367 terrain_attr=0000 terrain_camera=48
live_frames=15 live_dispatch=17 live_local=16 live_plan=1
live_state=1 live_handler=BF87FD
```

The validator still checks the existing dispatcher signature, frame layout, OAM/DMA, SPC driver image, fixed-point motion and IPL transfer results.

This is still not a complete playable or pixel-perfect port. The live bridge currently executes the normal grounded plan and airborne state-11 path; the complete original controller/state transition graph, wall/ceiling/material side effects, actors, progression, menus and full audio remain major blockers.

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

There are now **87 automated tests configured**: 86 C tests and one Python control-flow test. Fifteen focused local tests pass with `-Wall -Wextra -Wpedantic -Werror`, including ROM terrain placement, high-X levels, jumping/landing, live dispatcher transitions, authentic visual construction and the clean-room IPL bridge.

## Validate translated gameplay, terrain and SPC handoff

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

Q/E or the mapped L/R inputs accelerate the player; Z/B jumps. With the ROM-aware initializer, the player starts on original terrain and the frontend reports the current dispatcher state and terrain attributes.

## Accuracy boundary

Terrain shape equations and flags follow the existing clean-room translations of `$81:8409` and the level terrain records at `$81:8B94-$81:8C4A`. The live airborne path calls the translated state-11 handler rather than reimplementing its gravity separately. Material attributes are surfaced but many material-specific side effects remain intentionally unimplemented.

## What still blocks gameplay

- Expand the live bridge from states 1/11 to the complete controller-driven state transition graph.
- Implement wall, ceiling, neighboring-cell and material-specific collision side effects.
- Initialize the real two-player object pair and linked objects from level data.
- Translate common enemies, barrels, collectibles and level-completion logic.
- Continue the loaded SPC driver into its command/timer loop and implement DSP voices, BRR, music and effects.
- Implement menus, progression and SRAM compatibility.
- Compare native frame/input/state/OAM/VRAM/APUIO traces against an emulator reference.
