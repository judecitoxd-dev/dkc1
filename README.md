# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game identified by the user-provided cartridge dump. It is based on disassembly and behavioral analysis; it is decompilation-like engineering, not Rare's original source recovered line for line.

## Current status

**Overall engineering progress: 98%**

The percentage measures completed and tested engineering systems. It is not the percentage of gameplay currently playable and is not based on ROM bytes converted. The project remains at 98% because an original-compatible level loop, complete actor behavior, progression and audible DSP output are still absent.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 98% |
| Semantic portable C | player dispatcher + terrain envelope + common/live barrel bridge |
| PC rendering / widescreen | 99% infrastructure |
| Input / saves / menus / compatibility | 91% |
| Audio loading / driver path | 80% |

Every entry in the original 87-state player dispatcher remains classified: 84 states have executable local C semantics and states 0, 1 and 20 retain exact call plans. The live frontend currently executes the normal grounded plan and airborne state 11 using fixed-point movement and ROM terrain.

Terrain collision includes floor, slope, wall and ceiling probes. Ground, wall and ceiling attributes are retained separately, but material-specific water, damage, conveyor and neighboring-cell side effects are still unresolved.

## Confirmed object identities

The normal object type `$73` is the **Sign**. Its table callback is `$BF:8453`, which advances animation and enters the common object renderer. Supported-ROM validation leaves the Sign active; the older touch-deactivate option remains only as a portable test policy.

The barrel family is catalogued as follows:

| Type | Object | Callback | Idle animation |
|---:|---|---:|---:|
| `$22` | Steel Keg | `$BF:CF0C` | `$00D1` |
| `$23` | Barrel | `$BF:CF0C` | `$00D2` |
| `$24` | Rope Barrel | `$BF:CF0C` | `$00D3` |
| `$25` | Oil Drum | `$BF:83A0` | `$00D4` |
| `$26` | DK Barrel | `$BF:CF0C` | `$00D5` |
| `$27` | TNT Barrel | `$BF:CF0C` | `$00D6` |

`barrel_runtime` translates the local semantics of all ten states selected by `$BF:CF0C`. It models initialization, held/thrown paths, Steel Keg cleanup, DK Barrel release requests, TNT countdown/explosion requests, break sounds, explosion-script spawning and the Manky Kong reciprocal-link check. Untranslated helpers remain explicit required calls. The Oil Drum is excluded because it has a different callback.

## Live barrel bridge

`barrel_live_runtime` now services enough of those unresolved helpers to run a deterministic portable object loop:

```text
spawn
→ proximity pickup
→ held position follows player/facing
→ throw with 8.8 velocity
→ gravity and subpixel integration
→ floor/wall terrain callbacks
→ rolling or impact
→ common barrel dispatcher
→ break / TNT explosion / DK Barrel release request
```

The bridge preserves wrapped 16-bit world X, uses the translated fixed-point integrator, stores floor/wall material attributes and records pickups, throws, landings, wall hits, breaks, explosions and Kong-release requests.

Behavior confirmed by focused validation:

- A normal Barrel is picked up, carried, thrown and broken against a wall with sound `$14`.
- A Steel Keg survives the same wall collision and reverses horizontal velocity.
- A TNT Barrel landing requests destruction, explosion and two effect scripts.
- A DK Barrel landing requests destruction and releasing the trapped Kong.
- The Oil Drum is rejected because it uses its separate `$BF:83A0` runtime.

Pickup distance, held offset and the adapter that translates generic terrain callbacks into helper carries are explicitly portable bridge policy. They are not claimed to be exact original helper implementations yet. The next step is binding this module to real level object spawning, the frontend object scheduler, authentic barrel animation/OAM/DMA and player pickup/throw states.

The authentic player visual path remains connected:

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

The bounded SPC700 and clean-room IPL work remains: startup reaches `$FFC0`, the public transfer protocol can upload bytes and the loaded driver can be relaunched at `$05E8`. DSP music and effects are not implemented.

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

There are now **92 automated tests configured**: 91 C tests and one Python control-flow test. The new live-barrel test compiles and passes locally with `-Wall -Wextra -Wpedantic -Werror` and AddressSanitizer/UndefinedBehaviorSanitizer against a dispatcher-compatible focused harness. The complete 92-test repository suite was not rerun in this stage, and no remote CI result is claimed.

## Validate translated gameplay, objects and SPC handoff

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

Q/E or the mapped L/R inputs accelerate the player; Z/B jumps. With the ROM-aware initializer, the player starts on original terrain.

## Accuracy boundary

Object names are corroborated against a public symbolic DKC1 disassembly, while callbacks, state tables, local instructions, animations and deterministic behavior are checked against the supported Rev 2 ROM. No source or asset data from that project is copied into this repository.

## What still blocks a real 100%

- Bind the live barrel bridge to actual level object records and the interactive frontend.
- Use authentic barrel animation/OAM/DMA while held, thrown, rolling and breaking.
- Expand the live player bridge beyond states 1 and 11 and connect original pickup/throw transitions.
- Translate the separate Oil Drum callback and original behavior of enemies, collectibles, signs and level-completion actors.
- Implement confirmed material-specific collision effects.
- Initialize the real two-player object pair and linked objects from level data.
- Continue the loaded SPC driver into its command/timer loop and implement DSP, BRR, music and effects.
- Implement level completion, menus, progression and SRAM compatibility.
- Compare native frame/input/state/object/OAM/VRAM/APUIO traces against an emulator reference.
