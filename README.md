# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game
identified by the user-provided cartridge dump. It is based on disassembly and
behavioral analysis; it is decompilation-like engineering, not Rare's original
source recovered line for line.

## Current status

**Overall engineering progress: 99%**

**Gameplay status: early native prototype; the first level is not yet
completable.**

The percentage measures completed and tested engineering systems. It is not the
percentage of gameplay currently playable and is not based on ROM bytes
converted. One hundred percent remains reserved for an original-compatible,
playthrough-complete game with progression, saves and audible gameplay audio.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 99% |
| Semantic portable C | player, terrain, damage bridge, source Barrel, streamed Gnawty and camera object stream |
| PC rendering / widescreen | 99% infrastructure |
| Input / saves / menus / compatibility | 91% |
| Audio loading / driver path | 80% |

Every entry in the original 87-state player dispatcher remains classified: 84
states have executable local C semantics and states 0, 1 and 20 retain exact
call plans. The live frontend executes the grounded state-1 plan and airborne
state 11 using fixed-point movement and ROM terrain.

Terrain collision includes floor, slope, wall and ceiling probes. Ground, wall
and ceiling attributes are retained separately, but water, damage tiles,
conveyors and other material-specific effects remain incomplete.

## Source-driven level object import

The original level-object loader at `$BD:FDD9` (mirrored in bank `$FD`) is
translated far enough to read the normal eight-byte sprite records selected by
the pointer table at `$BD:8000`:

```text
word 0: loader command
word 1: world X
word 2: world Y
word 3: B5 object-definition address
```

Normal records use command `1`; a zero command terminates the list. The B5
definition reader applies field/value assignments and recovers the normal-object
type from field `$0D45`.

Command `$8200` is modeled as the call used by `$B5:8052`, not as a terminal
redirect: the nested definition runs, returns, and parsing resumes at the next
word pair in the caller. A call-stack cycle guard prevents recursive loops while
allowing shared definitions to be called independently. Unsupported B5 commands
remain an explicit boundary because their handlers use different record sizes.

A full Rev 2 ROM catalog regression across all 230 entrance IDs reports:

```text
normal records:      10357
resolved type IDs:   10356
unresolved:              1
catalog signature:  A5702FF2DA67FE40
```

For Jungle Hijinxs entrance `$0016`, deterministic validation reads:

```text
list source:        $BD:95DC
records:            66
list signature:     BE8955E9C89E92DD
Barrel record:      35
record source:      $BD:96F4
world X/Y:          $0986 / $005F
definition:         $B5:92A9
type assignment:    $0D45 = $0023
scheduler callback: $BF:CF0C
```

`level_object_spawn` places one selected type in the original primary object
pool and runs the translated scheduler to verify its callback and pass.
`level_object_import` resolves the complete normal-record list in original
order, imports the first 25 supported types into the real primary-slot window,
and explicitly counts unresolved, unsupported and overflow records.

## Camera-driven object lifecycle

`level_object_stream` builds a resolved catalog for every supported normal
record in an entrance. Each frontend frame now:

```text
camera envelope
→ retain visible records in stable slots
→ remove records that left the envelope
→ fill free primary slots in original list order
→ enforce the 25-slot primary pool
→ run and verify the scheduler callback for every active type
```

Retained objects keep their slot and scheduler state. Newly visible records
reuse free slots; excess visible records are reported instead of silently lost.
The whole-cartridge frontend validator includes active record, slot, type and
callback information in its signature.

The normal Barrel and the first active Gnawty continue into executable actor
runtimes. Other streamed types are catalogued, slotted and dispatched, but their
individual behavior and rendering remain to be connected.

## First live enemy: Gnawty

Gnawty type `$004D`, callback `$BF:840C`, and walk/turn/dead animation IDs
`$015A/$015B/$015C` are connected to a portable live runtime. The camera stream
selects the source record and original primary slot; the scheduler callback is
verified every frame before the enemy advances.

The current bridge provides:

```text
source Gnawty record
→ stable camera-stream slot
→ scheduler callback $BF:840C
→ walk animation script and original frame data
→ patrol motion with ROM-terrain wall/ledge probes
→ stomp defeat and player rebound
→ side-contact knockback and temporary invulnerability
→ defeated-record persistence
→ OAM and frame-graphics DMA
→ PC render
```

Side contact now feeds `player_combat_runtime`: a newly accepted hit applies
fixed-point horizontal/upward knockback, starts a bounded invulnerability timer,
ignores repeated overlap hits during that timer and flashes the player in the
software renderer. This is an explicit portable bridge rather than a claim that
the original shared damage helper, Kong swap and hurt-state timing are complete.

Patrol distance, collision boxes, stomp policy, knockback constants and the
invulnerability duration remain portable policy. Exact callback `$BF:840C`,
shared enemy helper semantics, Kong loss/swap and original turn/hurt timing
remain to be translated.

## Authentic Barrel path

```text
original level record
→ B5 definition/type assignment
→ primary object slot
→ scheduler callback $BF:CF0C
→ live pickup/hold/throw bridge
→ fixed-point motion and ROM terrain
→ original animation script
→ original frame layout
→ OAM and frame graphics DMA
→ PC render
```

For the normal Barrel in the supported Rev 2 ROM:

| Phase | Animation | First frame | Pieces | DMA bytes |
|---|---:|---:|---:|
| Idle | `$00D2` | `$1BD4` | 7 | 608 |
| Held | `$00D8` | `$1C18` | 6 | 576 |
| Thrown/rolling | `$00DE` | `$1BF8` | 6 | 576 |

Y remains a portable pickup/throw bridge until the exact original player carry
states and ownership helpers are connected. B continues to jump.

The bounded SPC700 and clean-room IPL work remains: startup reaches `$FFC0`, the
public transfer protocol can upload bytes and the loaded driver can be relaunched
at `$05E8`. DSP music and effects are not implemented.

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

There are now **103 automated tests configured**: 102 C tests and one Python
control-flow test. The new standalone player-combat regression was compiled and
executed with `-Wall -Wextra -Wpedantic -Werror`. The complete repository suite
and remote CI have not yet been confirmed after this integration.

## Validate all scene frontends

```bash
./build/dk1_frontend_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

The validator includes live Gnawty source index, motion, animation frame and
callback state in addition to the level-object and Barrel signatures.

## Interactive preview

When X11 is available:

```bash
./build/dk1_x11 \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0x16 384 224
```

For Jungle Hijinxs, the frontend imports the original object catalog, streams
visible objects, connects the first active Gnawty, and spawns the normal Barrel
from its source record. Q/E or mapped L/R accelerates the player, Z/B jumps and
U/Y picks up or throws.

## Accuracy boundary

The public symbolic DKC1 disassembly is used only to corroborate labels. Record
addresses, pointer traversal, definition assignments, callback selection,
animations, frames and deterministic signatures are checked against the
user-provided Rev 2 ROM. No ROM or extracted assets are committed.

## What still blocks a real 100%

- Translate the exact Gnawty callback/shared enemy helpers, original hurt states,
  Kong loss/swap and invulnerability timing.
- Bind remaining streamed enemies, collectibles, signs, effects and completion
  types to executable actor state machines, collisions and rendering.
- Connect exact original player pickup, carry and throw states/ownership links.
- Implement confirmed material-specific collision effects.
- Initialize the real two-Kong object pair and linked objects from level data.
- Continue the SPC driver command/timer loop and implement DSP, BRR, music and
  effects.
- Implement menus, progression, SRAM compatibility and a complete playthrough.
- Compare native frame/input/state/object/OAM/VRAM/APUIO traces against an
  emulator reference.
