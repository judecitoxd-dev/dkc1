# Progress accounting

## Current total: 99%

Ninety-nine percent means the engineering foundation now reaches original level
records, B5 type definitions, bounded object-pool import, camera-driven slot
lifecycle, scheduler dispatch and one complete visible Barrel runtime. It does
**not** mean the game is ninety-nine percent playable.

One hundred percent remains reserved for a complete original-compatible game
loop and playthrough.

## Completed in the current 99% stage

### Original level sprite-list parser

- Located the loader at `$FD:FDE9` and pointer table `$BD:8000`.
- Added the normal eight-byte record format: command, world X, world Y and B5
  definition address.
- Added zero-command termination and a deterministic whole-list signature.
- Added the B5 field/value definition interpreter, redirect following and type
  recovery from `$0D45`.

### Source record to scheduler

- Added `level_object_spawn` for one selected source record.
- Requires normal command `1`, resolves its B5 type, inserts it into the original
  scheduler and verifies callback plus object pass.
- Jungle Hijinxs record 35 resolves to Barrel `$23` and callback `$BF:CF0C`.

### Complete bounded object-list import

- Added `level_object_import`.
- Walks the complete entrance list in original order.
- Resolves every supported normal type and imports up to the original 25 primary
  slots in one deterministic scheduler snapshot.
- Preserves each record, definition, index, slot, expected callback and observed
  callback/pass.
- Explicitly counts non-normal, unresolved, unsupported and overflow records.

### Camera-driven lifecycle

- Added `level_object_stream`.
- Builds a catalog of every supported normal record in the entrance.
- Uses the current camera, viewport and margins to activate visible records.
- Retains visible records in stable primary slots and preserves their scheduler
  state.
- Removes records that leave the envelope and reuses their slots in original
  list order.
- Enforces the 25-slot primary pool and reports visible overflow.
- Runs and verifies the scheduler callback for every active record each frame.
- Connected the stream to `Dk1SoftwareFrontend`, so it advances whenever the
  camera advances.

### Level-aware frontend

- Initializes the complete bounded import and live camera stream before spawning
  the first normal Barrel into the executable scene runtime.
- Preserves import and stream counts in validation stats.
- Whole-cartridge validation now hashes active record indices, slots, types and
  callbacks and reports catalog/enter/exit/overflow totals.
- Entrances without a resolvable list or normal Barrel still initialize.

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

- Added `level_object_import` and `level_object_stream` tests.
- Configured validation increases from 97 to 99 tests: 98 C plus one Python.
- The new synthetic import and stream harnesses compile and execute in a focused
  build with `-Wall -Wextra -Wpedantic -Werror`.
- The complete repository suite and remote CI were not run in this stage.

## Required for 100%

- Bind the streamed enemy, collectible, sign, effect and level-completion types
  to executable actor state machines, collisions and visible rendering.
- Exact player carry/throw states and object ownership links.
- Material-specific collision behavior.
- Real two-Kong and linked-object initialization.
- Menus, progression and SRAM.
- SPC command execution, DSP/BRR and audible music/effects.
- A complete, repeatable playthrough with emulator-reference comparisons.
