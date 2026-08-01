# Progress accounting

## Current total: 99% engineering foundation

**Gameplay completeness is still an early prototype. Jungle Hijinxs is not yet
completable and the full game is not close to a playable 99%.**

The engineering percentage tracks foundational systems: original level records,
B5 type definitions, bounded object-pool import, camera-driven slot lifecycle,
ROM texture/palette loading and caching, scheduler dispatch, streamed
Barrel-family runtimes, the first streamed enemy family and a portable
player-damage bridge. It does **not** measure the fraction of levels, enemies,
menus, audio or progression that can be played.

One hundred percent remains reserved for a complete original-compatible game
loop and repeatable playthrough.

## Completed in the current 99% stage

### ROM texture and palette loading

- ROM identity validation is followed by real scene asset loading; the cartridge
  is not used only as a checksum gate.
- Reads each scene's original VRAM-package plan from the legal user-provided ROM.
- Applies direct ROM transfers and the translated DKC graphics decompressor into
  a host-side 64 KiB SNES VRAM image.
- Applies original CGRAM palette uploads, including inverted uploads and special
  palette patches.
- Background tiles, object/player frame graphics and palettes are decoded from
  those loaded VRAM/CGRAM images by the PC renderer.
- `Dk1SceneAssetStats` records package count, direct/compressed records, DMA and
  decompressed bytes, palette uploads/colors, nonzero VRAM/CGRAM content and
  deterministic VRAM/palette signatures.
- `dk1_scene_probe` renders a scene and reports the complete asset-load
  accounting; `dk1_scene_validate` totals loading across all scene IDs.

### Persistent local scene-asset cache

- Split scene initialization into ROM metadata preparation and graphics/palette
  population so a validated cache can skip repeated decompression.
- Added the versioned `DK1ASST1` binary format containing the initialized 64 KiB
  VRAM image, 256-color CGRAM image and portable load accounting.
- Every cache is keyed by a full-ROM fingerprint and size, level ID and scene
  option flags; the legal ROM is still loaded and validated on every run.
- Cache headers verify format version, payload dimensions, scene metadata and
  separate payload/VRAM/palette signatures.
- Missing cache files are generated after a normal ROM decode. Stale, truncated,
  mismatched or corrupted files are rejected and rebuilt from the ROM.
- Cache writes use a temporary file and rename; inability to write the optional
  optimization file does not prevent the scene from loading.
- Cache telemetry is excluded from deterministic scene signatures, so a fresh
  ROM decode and a verified cache hit must produce identical scene state.
- Windows and X11 automatically create per-level sidecars beside the ROM and
  display whether assets came from ROM, a newly saved cache or a cache hit.
- Generated `*.dk1-assets-*.bin` files are ignored by Git and no ROM or extracted
  assets are committed.
- The ROM-backed Jungle Hijinxs regression now requires a first-load cache write,
  a second-load cache hit and identical complete scene signatures.
- A ROM-independent regression verifies cache roundtrip, wrong-key rejection and
  corruption rejection using synthetic VRAM/CGRAM data.

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
- The frontend binds every visible Gnawty up to the original 25 primary object
  slots instead of bridging only one actor.
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

### Streamed Barrel family

- Added `level_barrel_pool`, bounded by the original 25 primary object slots.
- Connects visible Steel Kegs, normal Barrels, Rope Barrels, DK Barrels and TNT
  Barrels from their original level-list records.
- Keeps the first known Jungle Hijinxs Barrel embedded for compatibility with
  existing probes while all other supported source barrels use the new pool.
- Idle barrels follow source-record camera lifetime. A held, thrown or rolling
  barrel may continue beyond its source envelope until it is destroyed or moves
  outside a wider runtime envelope.
- Reuses the existing Barrel dispatcher, fixed-point motion, ROM terrain,
  original animation scripts, frame layouts, OAM and graphics DMA.
- Coordinates the Y-button across every Barrel runtime so only one actor may
  consume a pickup or throw action during a frame.
- Destroyed barrels mark their original source records as defeated and are not
  recreated by ordinary camera movement.
- Tracks entry, retention, exit, overflow, pickup, throw and destruction counts
  and contributes pointer-free state to the deterministic frontend signature.
- The ROM-backed frontend test sweeps Jungle Hijinxs and compares every active
  supported source record, excluding the compatibility Barrel, with the live
  streamed pool while requiring zero overflow.

### Portable frontend lifecycle

- `dk1_software_frontend_dispose` releases every dynamically streamed Gnawty and
  Barrel and is safe to call repeatedly or before initialization.
- Disposal clears active/overflow diagnostics so a restarted level begins from a
  deterministic zero-resource state.
- The Windows preview disposes streamed actors before replacing the level runtime
  and again during application shutdown.
- The X11 preview zero-initializes its frontend and performs the same cleanup on
  every exit path.
- ROM-independent regression tests allocate streamed actors, dispose them, check
  every slot and call disposal a second time.

### Player damage and invulnerability bridge

- Added `player_combat_runtime`.
- Gnawty side contact applies horizontal/upward fixed-point knockback.
- Starts a bounded invulnerability timer and ignores repeated overlap hits.
- Flashes the player in the software renderer while invulnerable.
- Tracks accepted hits, ignored hits and invulnerable frames in deterministic
  frontend state.
- This is portable policy; the exact shared damage helper, Kong loss/swap,
  original hurt states and timing remain untranslated.

### Level-aware frontend

- Initializes the bounded import and live camera stream.
- Loads or restores original scene texture packages and palettes before
  rendering.
- Connects active streamed Gnawties and supported Barrel-family records to
  executable runtimes within the original primary-pool limit.
- The current actors run through portable collision, original animation,
  frame-layout, OAM/DMA and PC rendering paths.

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

- Configured validation contains 108 tests: 107 C executables plus one Python
  CFG test.
- The public Linux workflow compiles every target and runs the 89 tests that do
  not require copyrighted ROM bytes; all 89 passed after the persistent-cache
  integration.
- Python tool syntax validation passes in the same workflow.
- The Windows x64 workflow configures with MSVC, builds `dk1_win32`, packages it
  and uploads the cache-enabled preview artifact successfully.
- The 19 ROM-backed tests remain configured and intentionally omitted from public
  CI. They run locally when `DK1_TEST_ROM` points to a legal USA Rev 2 cartridge
  image.
- The cache-enabled Windows artifact is `8823681077`, digest
  `sha256:7dde236d46e3b2d51c900b11a92a2a72658b84779bf2132907326aed900c2c20`.
- Remote CI validates compilation and the portable suite. Exact ROM-derived
  graphics values and first-load/cache-hit fidelity still require a locally
  supplied legal ROM.

## Required for a real 100%

- Translate remaining dynamic graphics upload paths used during transitions and
  gameplay; the persistent cache currently covers initialized scene VRAM/CGRAM.
- Translate the exact Gnawty callback/shared enemy helpers, original hurt states,
  Kong loss/swap and invulnerability timing.
- Bind the remaining streamed enemy types, collectibles, signs, effects and
  level-completion objects to executable actor state machines, collisions and
  visible rendering.
- Translate Oil Drum behavior and exact per-type Barrel material/collision rules.
- Exact player carry/throw states and object ownership links.
- Material-specific collision behavior.
- Real two-Kong and linked-object initialization.
- Menus, progression and SRAM.
- SPC command execution, DSP/BRR and audible music/effects.
- A complete repeatable playthrough with emulator-reference comparisons.
