# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 64%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 73% |
| Semantic portable C | 81% |
| PC rendering / widescreen | 87% |
| Input / saves / menus / compatibility | 62% |
| Audio command path | 22% |

The runtime still reconstructs and renders all 230 level/location scenes. The player path now includes the exact `$BF:B2C5` input fan-out, local semantics for 13 compact handlers in states 6–19, the `$BF:A132` interrupt guard, explicit translation coverage, and dynamic visual-column writes into reconstructed BG tilemap VRAM. The audio path can also read and fingerprint the exact 40-byte SPC bootstrap block uploaded from `$8A:A342`.

The player dispatcher was validated directly against the supported ROM:

```text
player_states=87 invalid=0 wrapper_plans=2 signature=2BF70E846ED72122
```

The existing whole-cartridge frontend validation remains:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

This is still not a complete playable or pixel-perfect port. Thirteen compact player states now have executable local semantics and two additional states have exact wrapper plans, but the movement-heavy handlers, original animation scripts and complete object/OAM construction remain pending.

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

There are now **66 automated tests configured**: 65 C tests and one Python control-flow test. The six tests introduced in this stage pass locally with warnings treated as errors.

## Validate the original player dispatcher

```bash
./build/dk1_player_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

## Validate translated gameplay coverage

```bash
./build/dk1_gameplay_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

Expected result:

```text
player_states=87 planned=15 local=13 invalid=0 translation=0197E755C75F01D8 apu_boot=F2BE1E6916EC4EC2
```

## Interactive preview

When X11 is available:

```bash
./build/dk1_x11 \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0 384 224
```

## Accuracy boundary

The player callback addresses, state table, compact state semantics, input action order, interrupt transitions, terrain samples, APU handshake constants, bootstrap source and mailbox token behavior are tied to confirmed ROM routines. Dynamic tile streaming writes authentic expanded map words into reconstructed VRAM, but exact DMA timing is still pending. The visible marker remains diagnostic and is not yet Donkey Kong's original animation or full physics.

## What still blocks gameplay

- Translate most of the 87 original player state handlers.
- Decode original player animation records and object-to-OAM construction.
- Complete neighboring-cell and material collision side effects.
- Reproduce exact dynamic VRAM streaming and object spawning while moving.
- Implement SPC700 program execution, music and sound synthesis.
- Translate enemies, barrels, collectibles, menus, progression and SRAM compatibility.
- Compare native frame traces against an emulator reference.
