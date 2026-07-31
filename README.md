# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 70%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 76% |
| Semantic portable C | 87% |
| PC rendering / widescreen | 89% |
| Input / saves / menus / compatibility | 65% |
| Audio command/bootstrap path | 35% |

The runtime reconstructs and renders all 230 level/location scenes. The original player dispatcher now has 27 states with executable local semantics and three additional exact plans. States 21–34 cover input/movement wrappers, scripted transitions, camera-relative launch behavior and movement initialization. The object animation path now interprets normal frame records and common control commands from the original bank-$BE scripts.

The uploaded 40-byte SPC700 bootstrap is no longer only identified: a bounded SPC700 instruction subset executes it, transfers bytes through the original port protocol and follows the indirect jump into an uploaded payload.

Current gameplay/audio validation for the supported ROM:

```text
player_states=87 planned=30 local=27 invalid=0 translation=1D1529F0FAF5E93F apu_boot=F2BE1E6916EC4EC2 animation=0330 spc_steps=40
```

The whole-cartridge frontend validation remains:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

This is still not a complete playable or pixel-perfect port. Movement-heavy states, complete animation command coverage, original object-to-OAM construction, SPC700 driver execution and DSP synthesis remain pending.

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

There are now **69 automated tests configured**: 68 C tests and one Python control-flow test. The three tests introduced in this stage pass locally, and the focused local workspace passes 36/36 tests.

## Validate translated gameplay and bootstrap execution

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

State addresses, wrapper order, local field mutations, animation timing/pointer behavior, the supported animation control commands and the SPC700 bootstrap opcodes are tied to confirmed cartridge routines. Complex paired-object animation commands are returned as unsupported instead of being guessed. The visible frontend marker remains diagnostic and is not yet Donkey Kong's original object-to-OAM output.

## What still blocks gameplay

- Translate the remaining 57 player states, especially movement-heavy handlers.
- Finish all original animation commands and object-to-OAM construction.
- Complete neighboring-cell and material collision side effects.
- Reproduce exact NMI/DMA scheduling and object spawning during scrolling.
- Load and execute the larger SPC700 driver, then implement DSP music and effects.
- Translate enemies, barrels, collectibles, menus, progression and SRAM compatibility.
- Compare native frame traces against an emulator reference.
