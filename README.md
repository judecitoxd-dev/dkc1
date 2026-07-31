# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 58%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 70% |
| Semantic portable C | 74% |
| PC rendering / widescreen | 84% |
| Input / saves / menus / compatibility | 62% |
| Audio command path | 15% |

The runtime still reconstructs and renders all 230 level/location scenes. This stage begins the original gameplay and audio paths: both player object callbacks (`$BF:84AB` and `$BF:84A4`), the exact 87-entry player state dispatcher at `$BF:84CA`, common horizontal-boundary/camera-lead/effect-scale routines, event-driven state transitions, three-point terrain contacts, host dynamic-column tracking, and the CPU-to-SPC700 boot/command mailbox.

The player dispatcher was validated directly against the supported ROM:

```text
player_states=87 invalid=0 wrapper_plans=2 signature=2BF70E846ED72122
```

The existing whole-cartridge frontend validation remains:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

This is still not a complete playable or pixel-perfect port. Only the player callback shell, state identity, two wrapper plans, several shared routines, and event transitions are translated; most of the 87 state handlers remain to be converted.

See [`docs/PROGRESS.md`](docs/PROGRESS.md), [`docs/PLAYER_AUDIO_STREAMING.md`](docs/PLAYER_AUDIO_STREAMING.md), [`docs/FRONTEND_RUNTIME.md`](docs/FRONTEND_RUNTIME.md), and [`docs/SCENE_RECONSTRUCTION.md`](docs/SCENE_RECONSTRUCTION.md).

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

There are now **60 automated tests configured**: 59 C tests and one Python control-flow test. The seven tests introduced in this stage pass locally with warnings treated as errors.

## Validate the original player dispatcher

```bash
./build/dk1_player_validate \
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

The player callback addresses, state table, boundary checks, camera-lead calculation, partner-dependent scale update, event transitions, terrain samples, APU handshake constants, and mailbox token behavior are tied to confirmed ROM routines. Dynamic streaming is a host cache/tracking layer built around the confirmed 32-pixel map units. The visible marker remains diagnostic and is not yet Donkey Kong's original animation or physics.

## What still blocks gameplay

- Translate most of the 87 original player state handlers.
- Decode original player animation records and object-to-OAM construction.
- Complete neighboring-cell and material collision side effects.
- Reproduce exact dynamic VRAM streaming and object spawning while moving.
- Implement SPC700 program execution, music and sound synthesis.
- Translate enemies, barrels, collectibles, menus, progression and SRAM compatibility.
- Compare native frame traces against an emulator reference.
