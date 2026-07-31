# Progress accounting

## Current total: 98%

Ninety-eight percent refers to completed engineering infrastructure and translated systems, not ninety-eight percent playable gameplay.

The public headline remains below 100% because a complete original-compatible level loop, original actor behaviors, progression and audible DSP output are still absent.

## Completed in the 97-to-98% stage

### Player floor/wall/ceiling envelope

- Added `dk1_rom_terrain_point_solid`, backed by the existing ROM collision map and translated shape curves.
- Added leading-side probes at lower body, center and upper body positions.
- Horizontal collisions restore the old wrapped 16-bit X coordinate and clear velocity, target velocity and subpixel X.
- Added three head probes that stop upward movement at solid cell undersides.
- Ground, wall and ceiling attributes are kept separately, along with left/right/ceiling flags and contact counters.
- Material changes are measurable, but material-specific damage, water and conveyor behavior is not invented.

### Scheduler-driven animated actor callback

- Added `object_actor_runtime` for original object type `$73` / callback `$BF:8453`.
- Runs the object through the original primary scheduler pass.
- Verifies the callback selected by the type table.
- Advances the translated animation interpreter.
- Builds the resulting frame through the confirmed OAM and frame-DMA paths.
- The validation frame is `$0330`: 12 pieces and 576 graphics bytes.
- Touch-deactivate is an explicitly clean-room interaction policy, not a claim about type `$73`'s original game identity.

## Validation

- Added `player_terrain_envelope` and `object_actor_runtime` tests.
- Focused validation increases from 15 to 17 passing tests with `-Wall -Wextra -Wpedantic -Werror`.
- Configured project validation increases from 87 to 89 tests: 88 C tests plus the Python control-flow test.
- The gameplay validator checks a solid ROM terrain point and the type-$73 scheduler/animation/OAM/DMA/touch pipeline.

## Next measurable targets

- Map material attribute values to their confirmed original side-effect handlers.
- Expand the live controller bridge beyond states 1 and 11.
- Identify and translate one original barrel or enemy state machine rather than using the generic animated-render actor callback.
- Continue the loaded SPC driver into its command/timer loop and implement DSP/BRR output.
- Implement level completion, menus, progression and SRAM compatibility.
