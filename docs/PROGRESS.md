# Progress accounting

## Current total: 99% engineering foundation

**Gameplay completeness is still an early prototype. Jungle Hijinxs is not yet
completable and the full game is not close to a playable 99%.**

The engineering percentage tracks foundational systems: original level records,
B5 type definitions, bounded object-pool import, camera-driven slot lifecycle,
scheduler dispatch, one complete visible Barrel runtime, the first streamed
enemy runtime and a portable player-damage bridge. It does **not** measure the
fraction of levels, enemies, menus, audio or progression that can be played.

One hundred percent remains reserved for a complete original-compatible game
loop and repeatable playthrough.

## Completed in the current 99% stage

### Original level sprite-list parser

- Located the loader at `$BD:FDD9` and pointer table `$BD:8000`.
- Added the normal eight-byte record format: command, world X, world Y and B5
  definition address.
- Added zero-command termination and deterministic signatures.
- Recovers the scheduler type from field `$0D45`.
- Corrected `$8200` from terminal redirect semantics to nested-call semantics:
  the callee runs and parsing resumes in the caller.
- Added a call-stack cycle guard and retained an explicit boundary at unsupported
  variable-size B5 commands.
- Full Rev 2 catalog validation resolves 10,356 of 10,357 normal records across
  230 IDs with signature `A5702FF2DA67FE40`.

### Source record to scheduler

- `level_object_spawn` selects a source record, resolves its B5 type, inserts it
  into the original scheduler and verifies callback plus object pass.
- Jungle Hijinxs record 35 resolves to Barrel `$23` and callback `$BF:CF0C`.

### Complete bounded object-list import

- `level_object_import` walks entrance records in original order.
- Resolves supported normal types and imports up to the original 25 primary
  slots in one deterministic scheduler snapshot.
- Preserves record, definition, index, slot and callback/pass metadata.
- Explicitly counts non-normal, unresolved, unsupported and overflow records.

### Camera-driven lifecycle

- `level_object_stream` catalogs supported normal records.
- Uses camera, viewport and margins to activate visible records.
- Retains stable slots, removes exited records and reuses free slots in list
  order.
- Enforces the 25-slot primary pool and verifies every active scheduler callback.
- The stream advances with `Dk1SoftwareFrontend` as the camera advances.

### First streamed enemy: Gnawty

- Catalogued Gnawty as normal object type `$004D` with callback `$BF:840C`.
- Connected original walk, turn and dead animation IDs `$015A/$015B/$015C`.
- Added `gnawty_runtime` with verified scheduler dispatch, fixed-point patrol,
  ROM-terrain wall/ledge probes, stomp defeat, player rebound and side-contact
  reporting.
- Uses the source record and stable camera-stream slot rather than a manual
  debug spawn.
- Preserves defeated source-record state so a stomped Gnawty is not recreated
  by ordinary camera streaming.
- Builds original frame layouts through OAM and frame-graphics DMA for PC
  rendering.

### Player damage and invulnerability bridge

- Added `player_combat_runtime`.
- Gnawty side contact now applies horizontal/upward fixed-point knockback.
- Starts a bounded invulnerability timer and ignores repeated overlap hits.
- Flashes the player in the software renderer while invulnerable.
- Tracks accepted hits, ignored hits and invulnerable frames in deterministic
  frontend state.
- This is portable policy; the exact shared damage helper, Kong loss/swap,
  original hurt states and timing remain untranslated.

### Level-aware frontend and Barrel

- Initializes the bounded import and live camera stream.
- Connects the first active streamed Gnawty to its executable runtime.
- Spawns the first normal Barrel source record into the executable scene runtime.
- The Barrel continues through pickup/hold/throw, fixed-point motion, ROM
  terrain, original animation, frame layout, OAM/DMA and PC rendering.

## Deterministic Jungle Hijinxs Barrel result

```text
entrance=0016
list=BD95DC
records=66
signature=BE8955E9C89E92DD
record=35
record_pc=BD96F4
x=0986
y=005F
definition=92A9
type=0023
slot=1
pass=primary-normal
callback=BFCF0C
```

## Validation

- Existing definition-stack and complete-catalog tests remain configured.
- Gnawty identity, runtime, streamed frontend integration and whole-cartridge
  signature reporting remain configured.
- Added `player_combat_runtime` coverage for knockback direction, accepted and
  ignored hits, invulnerability expiry and flashing policy.
- Configured validation increases from 102 to 103 tests: 102 C plus one Python.
- The standalone player-combat test compiles and executes with
  `-Wall -Wextra -Wpedantic -Werror`.
- The complete repository suite and remote CI have not yet been confirmed after
  this integration.

## Required for a real 100%

- Translate exact Gnawty callback/shared enemy helpers, original hurt states,
  Kong loss/swap and invulnerability timing.
- Support all simultaneously active enemies rather than one bridged Gnawty.
- Bind remaining streamed enemies, collectibles, signs, effects and
  level-completion types to executable actor state machines, collisions and
  visible rendering.
- Exact player carry/throw states and object ownership links.
- Material-specific collision behavior.
- Real two-Kong and linked-object initialization.
- Menus, progression and SRAM.
- SPC command execution, DSP/BRR and audible music/effects.
- A complete repeatable playthrough with emulator-reference comparisons.
