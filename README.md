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
| ROM texture/palette loading | working per scene, persistent cache still pending |
| Reset-vector and boot-entry analysis | 80% |
| Routine discovery and symbol map | 99% |
| Semantic portable C | player, terrain, damage bridge, streamed Barrel family, streamed Gnawty and camera object lifecycle |
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

## The ROM is also the asset source

The cartridge file is **not only checked for identity**. After validating the
USA Rev 2 image, each scene loader reads the original scene recipe and then:

```text
legal user ROM
→ scene VRAM-package list
→ direct ROM transfers and DKC decompression
→ host-side 64 KiB SNES VRAM image
→ original CGRAM palette uploads
→ tile, tilemap, sprite-frame and palette decoding
→ PC rendering
```

No converted graphics are committed to the repository. Background tiles,
object/player frame graphics and palettes are loaded from the user's ROM into
host memory when the scene starts. A future persistent local cache can avoid
repeating decompression, but it is an optimization rather than a requirement for
rendering.

`Dk1SceneAssetStats` records package count, direct/compressed records, DMA bytes,
decompressed bytes, palette uploads, nonzero VRAM/palette contents and stable
VRAM/CGRAM signatures. `dk1_scene_probe` prints those values and writes a PPM
render, while `dk1_scene_validate` totals asset loading across all scene IDs.
The X11 preview also reports whether textures and palettes were loaded.

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
record in an entrance. Each frontend frame performs:

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

Executable bindings currently cover every visible Gnawty and the supported
Barrel family. Other streamed types are catalogued, slotted and dispatched, but
their individual behavior and rendering remain to be connected.

## First live enemy family: Gnawty

Gnawty type `$004D`, callback `$BF:840C`, and walk/turn/dead animation IDs
`$015A/$015B/$015C` are connected to portable live runtimes. The camera stream
selects each source record and original primary slot; the scheduler callback is
verified before the enemy advances.

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

The frontend binds all visible Gnawties up to the original 25 primary slots.
Dynamically allocated actors are retained by source record, released when they
leave the camera envelope or finish defeat, and included in deterministic
signatures without hashing host pointer values.

Side contact feeds `player_combat_runtime`: a newly accepted hit applies
fixed-point horizontal/upward knockback, starts a bounded invulnerability timer,
ignores repeated overlap hits during that timer and flashes the player in the
software renderer. This remains a portable bridge rather than a claim that the
original shared damage helper, Kong swap and hurt-state timing are complete.

## Streamed Barrel family

The executable Barrel path is shared by a bounded camera-driven pool:

```text
original level record
→ B5 definition/type assignment
→ stable primary object slot
→ scheduler callback
→ pickup/hold/throw ownership bridge
→ fixed-point motion and ROM terrain
→ original animation script and frame layout
→ OAM and frame-graphics DMA
→ PC render
```

The pool supports Steel Kegs, normal Barrels, Rope Barrels, DK Barrels and TNT
Barrels. Oil Drum behavior remains a separate untranslated boundary.

The first known Jungle Hijinxs normal Barrel remains embedded for compatibility
with existing probes. Other supported visible records are bound to the new
25-slot pool. Idle barrels follow source-record camera lifetime; held, thrown or
rolling barrels may continue beyond the source envelope until destroyed or
outside a wider runtime envelope. A single Y-button action can be consumed by
only one Barrel runtime in a frame, including the compatibility Barrel.

Destroyed barrels mark their original source record and are not recreated by
ordinary camera movement. Pool lifecycle, pickup, throw, destruction and
overflow data participate in the deterministic frontend signature.

For the normal Barrel in the supported Rev 2 ROM:

| Phase | Animation | First frame | Pieces | DMA bytes |
|---|---:|---:|---:|---:|
| Idle | `$00D2` | `$1BD4` | 7 | 608 |
| Held | `$00D8` | `$1C18` | 6 | 576 |
| Thrown/rolling | `$00DE` | `$1BF8` | 6 | 576 |

Y remains a portable pickup/throw bridge until the exact original player carry
states and ownership helpers are connected. B continues to jump.

## Portable frontend lifecycle

`dk1_software_frontend_dispose` releases dynamically streamed Gnawties and
Barrels and is safe before initialization or after a previous cleanup. The
Windows frontend calls it before replacing a level runtime and during shutdown;
the X11 frontend uses the same cleanup on every exit path.

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

There are **107 automated tests configured**: 106 C executables and one Python
control-flow test. Public CI compiles every target and runs the 88 tests that do
not require copyrighted ROM bytes; all 88 currently pass. The 19 ROM-backed
fidelity tests, including the new scene asset-loading regression, remain
configured for a locally supplied legal USA Rev 2 ROM.

The Windows x64 workflow configures with MSVC, builds `dk1_win32`, packages the
preview and uploads a ZIP artifact. The Linux workflow also checks Python tool
syntax.

## Inspect a rendered scene and asset load

```bash
./build/dk1_scene_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0x16 0 0 384 224 jungle.ppm
```

The output reports whether textures and palettes were loaded, VRAM package and
record counts, compressed/direct transfers, byte totals and deterministic asset
signatures.

## Validate all scene frontends

```bash
./build/dk1_frontend_validate \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc"
```

The validator includes camera-object catalog state, Gnawty actors, Barrel-family
runtime state, player combat state and original rendering-path signatures.

## Interactive preview

When X11 is available:

```bash
./build/dk1_x11 \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  0x16 384 224
```

For Jungle Hijinxs, the frontend loads the original texture packages and
palettes, imports the original object catalog, streams visible objects and
connects visible Gnawties and supported Barrel-family source records. Q/E or
mapped L/R accelerates the player, Z/B jumps and U/Y picks up or throws.

## Accuracy boundary

The public symbolic DKC1 disassembly is used only to corroborate labels. Record
addresses, pointer traversal, definition assignments, callback selection,
animations, frames and deterministic signatures are checked against the
user-provided Rev 2 ROM. No ROM or extracted assets are committed.

## What still blocks a real 100%

- Add a persistent local asset cache and complete the remaining dynamic graphics
  upload paths used during gameplay transitions.
- Translate the exact Gnawty callback/shared enemy helpers, original hurt states,
  Kong loss/swap and invulnerability timing.
- Bind remaining streamed enemies, collectibles, signs, effects and completion
  types to executable actor state machines, collisions and rendering.
- Translate Oil Drum behavior and exact per-type Barrel material rules.
- Connect exact original player pickup, carry and throw states/ownership links.
- Implement confirmed material-specific collision effects.
- Initialize the real two-Kong object pair and linked objects from level data.
- Continue the SPC driver command/timer loop and implement DSP, BRR, music and
  effects.
- Implement menus, progression, SRAM compatibility and a complete playthrough.
- Compare native frame/input/state/object/OAM/VRAM/APUIO traces against an
  emulator reference.
