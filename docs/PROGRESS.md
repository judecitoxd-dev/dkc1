# Progress accounting

## Current total: 98% — held intentionally

This stage does **not** increase the headline percentage. Ninety-eight percent
refers to engineering infrastructure and translated systems, not ninety-eight
percent playable gameplay. Reaching 100% now requires a complete original-
compatible level loop rather than more isolated tables.

## Completed while remaining at 98%

### Correct identity for type `$73`

- Confirmed normal object type `$73` as the Sign.
- Preserved its table callback `$BF:8453` and the existing scheduler → animation
  → OAM/DMA path.
- Removed the portable touch-deactivate policy from the supported-ROM actor
  test and gameplay validator; the Sign remains active after contact.
- Kept the policy available only as an explicitly non-original host/test option.

### Confirmed barrel family catalog

- Added identities for Steel Keg `$22`, Barrel `$23`, Rope Barrel `$24`, Oil Drum
  `$25`, DK Barrel `$26` and TNT Barrel `$27`.
- Recorded idle animation IDs `$D1-$D6`.
- Verified that `$22/$23/$24/$26/$27` use callback `$BF:CF0C`.
- Recorded the Oil Drum's separate callback `$BF:83A0` and excluded it from the
  common dispatcher runtime.

### Common barrel state dispatcher

- Added `barrel_runtime` for the ten-state table at `$BF:CF17`.
- Implemented local semantics for handlers `$BF:CF2B`, `$CF38`, `$CF3F`,
  `$CFCC`, `$D117`, `$D156`, `$D185`, `$D198`, `$D1C7` and `$D1DD`.
- Covered initialization, motion/animation/render paths, Steel Keg cleanup,
  break/despawn paths, TNT countdown/explosion, two explosion-script requests,
  DK Barrel Kong release requests and Manky Kong reciprocal-link validation.
- Exposed unresolved JSR/JSL helpers through a 64-bit required-call mask.

## Validation

- Added `object_identity` and `barrel_runtime` test targets.
- Configured project validation increases from 89 to 91 tests: 90 C tests plus
  the Python control-flow test.
- Both new tests pass locally with `-Wall -Wextra -Wpedantic -Werror`.
- Supported-ROM gameplay validation now expects an active Sign and deterministic
  TNT/DK Barrel results rather than a fake Sign disappearance.
- The complete 91-test suite and remote CI were not run in this stage.

## Next measurable targets

- Connect one common Barrel object to the live scheduler, terrain collision,
  player pickup/throw state and authentic animation/OAM/DMA path.
- Translate the Oil Drum's separate callback.
- Expand the live player controller graph beyond states 1 and 11.
- Implement material side effects and level completion.
- Continue the SPC driver command/timer path and DSP output.
