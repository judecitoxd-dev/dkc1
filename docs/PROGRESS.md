# Progress accounting

## Current total: 99%

Ninety-nine percent means the engineering foundation reaches original level
records, B5 type definitions, bounded object-pool import, camera-driven slot
lifecycle, scheduler dispatch and one complete visible Barrel runtime. It does
**not** mean the game is ninety-nine percent playable.

One hundred percent remains reserved for a complete original-compatible game
loop and playthrough.

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
- Full Rev 2 catalog validation now resolves 10,356 of 10,357 normal records
  across 230 IDs with signature `A5702FF2DA67FE40`.

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

### Level-aware frontend and Barrel

- Initializes the bounded import and live camera stream.
- Spawns the first normal Barrel source record into the executable scene runtime.
- Continues through pickup/hold/throw, fixed-point motion, ROM terrain, original
  animation, frame layout, OAM/DMA and PC rendering.

## Deterministic Jungle Hijinxs source result

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

- Added `level_sprite_definition_stack` and `level_sprite_catalog` tests.
- Configured validation increases from 99 to 101 tests: 100 C plus one Python.
- Both new tests pass locally with `-Wall -Wextra -Wpedantic -Werror`.
- The complete repository suite and remote CI were not run in this stage.

## Required for 100%

- Bind streamed enemy, collectible, sign, effect and level-completion types to
  executable actor state machines, collisions and visible rendering.
- Exact player carry/throw states and object ownership links.
- Material-specific collision behavior.
- Real two-Kong and linked-object initialization.
- Menus, progression and SRAM.
- SPC command execution, DSP/BRR and audible music/effects.
- A complete repeatable playthrough with emulator-reference comparisons.
