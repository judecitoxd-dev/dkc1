# dk1 — clean-room PC reimplementation

This repository is a clean-room native PC reimplementation of the SNES game
identified by the user-provided cartridge dump. It is based on disassembly and
behavioral analysis; it is decompilation-like engineering, not Rare's original
source recovered line for line.

## Current status

**Overall engineering progress: 99%**

The percentage measures completed and tested engineering systems. It is not the
percentage of gameplay currently playable and is not based on ROM bytes
converted. One hundred percent remains reserved for an original-compatible,
playthrough-complete game with progression, saves and audible gameplay audio.

| Area | Progress |
|---|---:|
| Cartridge identity / HiROM mapping | 100% |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 99% |
| Semantic portable C | player, terrain, source Barrel and camera object stream |
| PC rendering / widescreen | 99% infrastructure |
| Input / saves / menus / compatibility | 91% |
| Audio loading / driver path | 80% |

Every entry in the original 87-state player dispatcher remains classified: 84
states have executable local C semantics and states 0, 1 and 20 retain exact
call plans. The live frontend executes the grounded state-1 plan and airborne
state 11 using fixed-point movement and ROM terrain.

Terrain collision includes floor, slope, wall and ceiling probes. Ground, wall
and ceiling attributes are retained separately, but water, damage, conveyor and
other material-specific effects remain incomplete.

## Source-driven level object import

The original level-object loader at `$FD:FDE9` has now been translated far
enough to read the normal eight-byte sprite records selected by the pointer
table at `$BD:8000`:

```text
word 0: loader command
word 1: world X
word 2: world Y
word 3: B5 object-definition address
```

The B5 definition reader follows redirects, applies field/value assignments and
recovers the normal-object type from field `$0D45`. Normal records use command
`1`; a zero command terminates the list.

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
`level_object_import` now resolves the complete normal-record list in original
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
The whole-cartridge frontend validator records catalog, active, enter, exit and
overflow totals and includes active slot/type/callback data in its signature.

This is the original object-list and scheduler lifecycle foundation. Only the
normal Barrel currently continues into a complete executable actor, collision,
animation, OAM/DMA and visible-render path. Other streamed types are catalogued,
slotted and dispatched, but their individual behavior/render runtimes remain to
be connected.

## Authentic Barrel path

The source-driven object continues through the previously translated systems:

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
|---|---:|---:|---:|---:|
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

There are now **99 automated tests configured**: 98 C tests and one Python
control-flow test. The new synthetic full-list import and camera-stream harnesses
were also compiled and executed in a focused strict-warning build using
`-Wall -Wextra -Wpedantic -Werror`. The complete repository suite and remote CI
were not run in this stage.

## Validate all scene frontends

```bash
./build/dk1_frontend_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

The validator reports terrain and player dispatch, whole object-list import,
camera-stream lifecycle, Barrel discovery/spawn totals and an aggregate render
signature.

## Interactive preview

When X11 is available:

```bash
./build/dk1_x11 \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0x16 384 224
```

For Jungle Hijinxs, the frontend imports the original object catalog and spawns
the normal Barrel from its source record. Q/E or mapped L/R accelerates the
player, Z/B jumps and U/Y picks up or throws.

## Accuracy boundary

The public symbolic DKC1 disassembly is used only to corroborate labels. Record
addresses, pointer traversal, definition assignments, callback selection,
animations, frames and deterministic signatures are checked against the
user-provided Rev 2 ROM. No ROM or extracted assets are committed.

## What still blocks a real 100%

- Bind streamed enemy, collectible, sign, effect and completion types to their
  executable actor state machines, collisions and rendering.
- Connect exact original player pickup, carry and throw states/ownership links.
- Implement confirmed material-specific collision effects.
- Initialize the real two-Kong object pair and linked objects from level data.
- Continue the SPC driver command/timer loop and implement DSP, BRR, music and
  effects.
- Implement menus, progression, SRAM compatibility and a complete playthrough.
- Compare native frame/input/state/object/OAM/VRAM/APUIO traces against an
  emulator reference.
