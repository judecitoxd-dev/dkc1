# Progress accounting

## Current total: 99%

Ninety-nine percent means the engineering foundation now reaches one authentic
level object from its original ROM record through type definition, scheduler,
terrain, animation, OAM/DMA and frontend rendering. It does **not** mean the game
is ninety-nine percent playable.

One hundred percent remains reserved for a complete original-compatible game
loop and playthrough.

## Completed in the 98-to-99% stage

### Original level sprite-list parser

- Located the loader at `$FD:FDE9` and pointer table `$BD:8000`.
- Added the normal eight-byte record format: command, world X, world Y and B5
  definition address.
- Added zero-command termination and a deterministic whole-list signature.
- Added the B5 field/value definition interpreter, redirect following and type
  recovery from `$0D45`.

### Source record to scheduler

- Added `level_object_spawn`.
- Requires normal command `1` before creating a primary object.
- Resolves the object type through its B5 definition.
- Inserts the type into the original scheduler state and verifies the callback
  and object pass.
- Jungle Hijinxs record 35 resolves to Barrel `$23` and callback `$BF:CF0C`.

### Level-aware frontend

- Added `level_software_frontend`.
- Initializes the regular ROM-aware frontend and imports the first normal Barrel
  record from the requested entrance.
- Preserves source address, record index, coordinates, definition, slot, pass and
  callback in validation stats.
- Updated X11 and whole-cartridge frontend validation to use the level-aware
  initializer.
- Entrances without a normal Barrel still initialize successfully.

## Deterministic Jungle Hijinxs result

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

- Added `level_sprite_records`, `level_object_spawn` and
  `level_software_frontend` tests.
- Configured validation increases from 94 to 97 tests: 96 C plus one Python.
- Parser and scheduler tests pass locally with strict warnings and ASan/UBSan.
- The level-aware wrapper passes a focused source-selection/integration harness.
- The full repository suite and remote CI were not run in this stage.

## Required for 100%

- Full object-list import and lifecycle.
- Exact player carry/throw states and object ownership links.
- Enemies, collectibles, effects and level completion.
- Menus, progression and SRAM.
- SPC command execution, DSP/BRR and audible music/effects.
- A complete, repeatable playthrough with emulator-reference comparisons.
