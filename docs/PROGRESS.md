# Progress accounting

## Current total: 97%

Ninety-seven percent refers to completed engineering infrastructure and translated systems, not ninety-seven percent playable gameplay.

The public headline is intentionally held below 100% because the project still lacks a complete original-compatible level loop, actor set, progression and audio output.

## Completed in the 96-to-97% stage

### Original ROM terrain contacts

- Added `player_terrain_runtime`, backed by the existing level terrain configuration, ROM collision blocks and 64 translated shape curves.
- Samples left, center and right foot positions and selects the closest valid support surface.
- Places the preview on an authentic surface and derives a vertical camera value from the 512-pixel collision space.
- Resolves falling crossings and small slope changes, clearing vertical velocity/subpixels on landing.
- Propagates the original descriptor/shape attributes into the player runtime for later material-specific behavior.

### Correct unsigned world X

- Changed the host player world-X representation from signed to unsigned 16-bit.
- Preserves the original wrapped 16-bit integration while supporting camera/map coordinates above `$8000`.
- Added coverage using terrain profile 1 at world X `$8630`, corresponding to a level region above the signed boundary.

### Live dispatcher bridge

- Added `player_live_runtime` and connected it to `software_frontend_step`.
- Grounded frames are identified with state 1 and its exact handler `$BF:87FD` plan.
- A jump enters state 11 and executes the translated `$BF:8FA7` local handler.
- The handler's `MOVE` request is satisfied by the fixed-point movement and terrain resolver.
- Landing transitions the live bridge back to state 1.

### ROM-aware frontend initialization

- Added `dk1_software_frontend_init_with_rom`.
- X11, frontend probe and whole-cartridge frontend validation can bind terrain before the first frame.
- The old ROM-less initializer remains for deterministic synthetic tests and fallback use.
- The object renderer uses a 512-pixel vertical origin when authentic terrain is active and retains 224 for the legacy fallback.

## Validation

- Added `player_terrain_runtime`, `player_live_runtime` and `software_frontend_terrain` test targets.
- Configured validation increases from 84 to 87 tests.
- Fifteen focused tests pass locally with `-Wall -Wextra -Wpedantic -Werror`.
- The gameplay validator checks an authentic terrain jump/landing trace and the state-11-to-state-1 transition.
- The frontend validator now reports how many of the 230 scenes bind ROM terrain and execute a local state on its test frame; its aggregate signature must be regenerated because runtime state changed.

## Next measurable targets

- Add wall/ceiling and material-specific side effects to the terrain bridge.
- Drive additional translated states from controller input and collision results.
- Translate one common barrel or enemy through scheduler, collision, animation, DMA and OAM.
- Continue from the IPL transfer into the loaded driver's command/timer loop.
- Begin level completion, menus, progression and SRAM compatibility.
