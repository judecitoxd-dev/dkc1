# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 77%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 80% |
| Semantic portable C | 93% |
| PC rendering / widescreen | 90% |
| Input / saves / menus / compatibility | 65% |
| Audio loading / bootstrap path | 48% |

The runtime reconstructs and renders all 230 level/location scenes. The original player dispatcher now has **42 states with executable local semantics** and three additional exact plans. States 35–49 add fixed-point launch integration, linked-object states, scripted transitions, gravity helpers, camera exits and paired-player reset behavior.

The original bank-$BE animation interpreter now supports forward and reverse frame playback plus all commands `$80-$91`, including the linked-object commands `$85-$8D`. Resolved primary, secondary and direct object links receive their original frame, motion and animation-speed writes.

The audio path now parses the exact 27-entry source table at `$8A:B15E`. Every entry is decoded as the four-byte block header consumed by `$8A:B4EB`: little-endian length, SPC destination and payload. All 27 blocks load into SPC RAM successfully, totaling 65,195 payload bytes.

Current gameplay/audio validation for the supported ROM:

```text
player_states=87 planned=45 local=42 invalid=0 translation=C56431FA86A799D5 apu_boot=F2BE1E6916EC4EC2 animation=0330 spc_steps=40 apu_sources=27 apu_bytes=65195 apu_catalog=D66EA2B33D4342BD apu_payload=6CC614324FAED0E8
```

The whole-cartridge frontend validation remains:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

This is still not a complete playable or pixel-perfect port. Forty-two player states remain untranslated, original frame ids are not yet converted into the complete OAM-piece layouts, and the larger SPC700 drivers are loaded but not executed by a complete CPU/DSP implementation.

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

There are now **71 automated tests configured**: 70 C tests and one Python control-flow test. The two new test targets and the expanded animation test pass locally; the focused local workspace passes 38/38 tests.

## Validate translated gameplay and audio loading

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

State addresses and local mutations are tied to confirmed bank-$BF routines. External helpers are surfaced as required-call flags rather than silently skipped. Animation command layouts, reverse traversal and linked-object writes follow `$BE:80E1-$BE:83CA`. Audio headers and payload movement follow `$8A:B1CB` and `$8A:B4EB-$8A:B538`. Exact DMA timing, complete object-to-OAM construction, larger SPC700 execution and DSP synthesis remain pending.

## What still blocks gameplay

- Translate the remaining 42 player states, especially the largest movement/collision handlers.
- Resolve original frame ids into complete multi-piece OAM layouts.
- Complete neighboring-cell and material collision side effects.
- Reproduce exact NMI/DMA scheduling and object spawning during scrolling.
- Execute the loaded SPC700 drivers and implement DSP music/effects.
- Translate enemies, barrels, collectibles, menus, progression and SRAM compatibility.
- Compare native frame traces against an emulator reference.
