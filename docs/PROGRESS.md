# Progress accounting

## Current total: 99% engineering foundation

**Gameplay completeness is still an early prototype. Jungle Hijinxs is not yet
completable and the full game is not close to a playable 99%.**

The engineering percentage tracks foundational systems: original level records,
B5 type definitions, bounded object-pool import, camera-driven slot lifecycle,
scheduler dispatch, one complete visible Barrel runtime, the first streamed
enemy family and a portable player-damage bridge. It does **not** measure the
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

### First streamed enemy family: Gnawty

- Catalogued Gnawty as normal object type `$004D` with callback `$BF:840C`.
- Connected original walk, turn and dead animation IDs `$015A/$015B/$015C`.
- Added `gnawty_runtime` with verified scheduler dispatch, fixed-point patrol,
  ROM-terrain wall/ledge probes, stomp defeat, player rebound and side-contact
  reporting.
- Uses source records and stable camera-stream slots rather than manual debug
  spawns.
- The frontend now binds every visible Gnawty up to the original 25 primary
  object slots instead of bridging only one actor.
- Additional runtimes exist only while their source records are streamed; actors
  are released when they leave the camera envelope or are defeated.
- Preserves defeated source-record state so stomped Gnawties are not recreated by
  ordinary camera streaming.
- Builds original frame layouts through OAM and frame-graphics DMA for PC
  rendering.
- The ROM-backed frontend test sweeps the complete Jungle Hijinxs camera range,
  compares active source records with bound runtimes and requires zero Gnawty
  capacity overflow.
- Active-runtime counts are refreshed after every enemy step, including the same
  frame in which an actor finishes its defeated state and is released.

### Portable frontend lifecycle

- `dk1_software_frontend_dispose` releases every dynamically streamed Gnawty and
  is safe to call repeatedly or before initialization.
- Disposal clears active/overflow diagnostics so a restarted level begins from a
  deterministic zero-resource state.
- The Windows preview disposes streamed actors before replacing the level runtime
  and again during application shutdown.
- The X11 preview now zero-initializes its frontend and performs the same cleanup
  on every exit path.
- A ROM-independent regression test allocates streamed actors, disposes them,
  checks every slot and calls disposal a second time.

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
- Connects every active streamed Gnawty to executable enemy runtimes within the
  original primary-pool limit.
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

- Configured validation contains 105 tests: 104 C executables plus one Python
  CFG test.
- The public Linux workflow compiles every target and runs the 87 tests that do
  not require copyrighted ROM bytes; all 87 passed after the frontend-lifecycle
  integration.
- The scheduler special-pass fixture agrees with the dispatch table: types 1 and
  2 both carry the special-pass attribute and both callbacks are visited.
- Python tool syntax validation passes in the same workflow.
- The Windows x64 workflow configures with MSVC, builds `dk1_win32`, packages it
  and uploads the preview artifact successfully after the restart-cleanup change.
- The 18 ROM-backed tests remain configured and are intentionally omitted from
  public CI. They run locally when `DK1_TEST_ROM` points to a legal USA Rev 2
  cartridge image.
- Remote CI therefore validates the portable implementation and build system,
  while source-ROM fidelity tests still require a locally supplied legal ROM.

## Required for a real 100%

- Translate the exact Gnawty callback/shared enemy helpers, original hurt states,
  Kong loss/swap and invulnerability timing.
- Bind the remaining streamed enemy types, collectibles, signs, effects and
  level-completion objects to executable actor state machines, collisions and
  visible rendering.
- Exact player carry/throw states and object ownership links.
- Material-specific collision behavior.
- Real two-Kong and linked-object initialization.
- Menus, progression and SRAM.
- SPC command execution, DSP/BRR and audible music/effects.
- A complete repeatable playthrough with emulator-reference comparisons.
