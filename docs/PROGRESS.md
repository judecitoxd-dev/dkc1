# Progress accounting

## Current total: 98% — held intentionally

This stage does **not** increase the headline percentage. Ninety-eight percent refers to engineering infrastructure and translated systems, not ninety-eight percent playable gameplay. Reaching 100% now requires a complete original-compatible level loop rather than more isolated modules.

## Completed while remaining at 98%

### Live common-barrel bridge

- Added `barrel_live_runtime` on top of the ten-state `$BF:CF0C` dispatcher.
- Added deterministic spawning for Steel Keg, Barrel, Rope Barrel, DK Barrel and TNT Barrel.
- Added proximity pickup and a held phase that follows the player's wrapped 16-bit world position and facing direction.
- Added throwing with `$0300` horizontal and vertical 8.8 velocities.
- Added barrel gravity `-$0070` with a `-$0600` terminal value and reused the translated fixed-point integration helpers.
- Added generic terrain callbacks for crossed floors and solid wall probes, preserving contact attributes.
- Added rolling after a survivable landing and collision dispatch through the existing `D23C`/`D324` result paths.
- Normal Barrel wall impact produces destruction and sound `$14`.
- Steel Keg wall impact reverses horizontal velocity instead of destroying the object.
- TNT landing produces explosion/destruction and two effect-script requests.
- DK Barrel landing produces destruction and a trapped-Kong release request.
- The Oil Drum is explicitly rejected because its callback is `$BF:83A0`, outside the common dispatcher.

### Accuracy boundary

Pickup range, held offsets and the adapter from portable terrain callbacks to unresolved original helper carries are host-bridge policy. They are measurable and deterministic, but not claimed to be exact original helper semantics. No percentage increase is justified until the bridge is connected to real level object spawning, the frontend scheduler, player pickup/throw states and authentic object rendering.

## Validation

- Added `barrel_live_runtime` to `dk1_core` and `barrel_live_runtime` to the configured tests.
- Configured validation increases from 91 to 92 tests: 91 C tests plus the Python control-flow test.
- The focused live-barrel test passes with `-Wall -Wextra -Wpedantic -Werror` and ASan/UBSan against a dispatcher-compatible local harness.
- The full repository suite and remote CI were not run in this stage.

## Next measurable targets

- Bind the live bridge to level object records and the interactive frontend.
- Build authentic barrel frames/OAM/DMA for each live phase.
- Connect player pickup, hold and throw states to the live dispatcher.
- Translate the Oil Drum's separate callback.
- Continue material effects, level completion and SPC/DSP output.
