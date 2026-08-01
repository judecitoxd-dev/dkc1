# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game
identified by the user-provided cartridge dump. It is based on disassembly and
behavioral analysis; it is decompilation-like engineering, not Rare's original
source recovered line for line.

## Current status

**Overall engineering progress: 98%**

The percentage measures completed and tested engineering systems. It is not the
percentage of gameplay currently playable and is not based on ROM bytes
converted. The project stays at 98% in this stage because an original-compatible
level loop, complete actor behavior, progression and audible DSP output are
still absent.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 98% |
| Semantic portable C | player dispatcher + terrain envelope + common barrel dispatcher |
| PC rendering / widescreen | 99% infrastructure |
| Input / saves / menus / compatibility | 91% |
| Audio loading / driver path | 80% |

Every entry in the original 87-state player dispatcher remains classified: 84
states have executable local C semantics and states 0, 1 and 20 retain exact
call plans. The live frontend currently executes the normal grounded plan and
airborne state 11, using fixed-point motion and ROM terrain.

Terrain collision includes floor, slope, wall and ceiling probes. Ground, wall
and ceiling attributes are retained separately, but material-specific water,
damage, conveyor and neighboring-cell side effects are still unresolved.

## Confirmed object identities

The former generic type-`$73` actor is now identified as the **Sign**. Its type
table callback remains `$BF:8453`, which advances animation and enters the common
object renderer. Supported-ROM validation no longer applies the portable
touch-deactivate test policy to this object: the Sign remains active.

The common barrel family is now catalogued:

| Type | Object | Callback | Idle animation |
|---:|---|---:|---:|
| `$22` | Steel Keg | `$BF:CF0C` | `$00D1` |
| `$23` | Barrel | `$BF:CF0C` | `$00D2` |
| `$24` | Rope Barrel | `$BF:CF0C` | `$00D3` |
| `$25` | Oil Drum | `$BF:83A0` | `$00D4` |
| `$26` | DK Barrel | `$BF:CF0C` | `$00D5` |
| `$27` | TNT Barrel | `$BF:CF0C` | `$00D6` |

`barrel_runtime` translates the local semantics of all ten states selected by
the common `$BF:CF0C` dispatcher. It models initialization, animation/render
paths, held/thrown transitions, Steel Keg cleanup, DK Barrel release requests,
TNT countdown/explosion requests, break sounds, explosion-script spawning and
the reciprocal Manky Kong link check. Every helper not yet translated is exposed
through an explicit required-call mask. The Oil Drum is excluded because it has
a different callback.

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

The bounded SPC700 and clean-room IPL work remains: startup reaches `$FFC0`, the
public transfer protocol can upload bytes and the loaded driver can be relaunched
at `$05E8`. DSP music and effects are not implemented.

Current supported-ROM gameplay validation additionally checks:

```text
sign_callback=BF8453 sign_pieces=12 sign_dma=576 sign_active=1
barrel_states=10 barrel_callback=BFCF0C
barrel_explode=1 barrel_spawns=2 barrel_release=1
```

The validator still checks the existing player-dispatch signature, terrain,
frame layout, OAM/DMA, SPC driver image, fixed-point motion and IPL transfer.

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

There are now **91 automated tests configured**: 90 C tests and one Python
control-flow test. The two new focused tests for object identity and the common
barrel dispatcher pass locally with `-Wall -Wextra -Wpedantic -Werror`. The
entire 91-test project was not rerun in this stage, and no remote CI result is
claimed.

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

Q/E or the mapped L/R inputs accelerate the player; Z/B jumps. With the
ROM-aware initializer, the player starts on original terrain.

## Accuracy boundary

Object names are corroborated against a public symbolic DKC1 disassembly, while
callbacks, state tables, local instructions, animations and deterministic
behavior are checked against the supported Rev 2 ROM. No source or asset data
from that project is copied into this repository.

## What still blocks a real 100%

- Expand the live player bridge beyond states 1 and 11.
- Connect the common barrel dispatcher to live object spawning, collision,
  ownership, pickup and throwing.
- Translate the separate Oil Drum callback and original behavior of enemies,
  collectibles, signs and level-completion actors.
- Implement confirmed material-specific collision effects.
- Initialize the real two-player object pair and linked objects from level data.
- Continue the loaded SPC driver into its command/timer loop and implement DSP,
  BRR, music and effects.
- Implement level completion, menus, progression and SRAM compatibility.
- Compare native frame/input/state/object/OAM/VRAM/APUIO traces against an
  emulator reference.
