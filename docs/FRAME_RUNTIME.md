# Native frame runtime

This stage connects previously isolated clean-room models into a host-executable frame path. It still uses no original ROM bytes, graphics, music, or level geometry.

## Input

`$00:C180-$00:C1AF` reads the SNES auto-joypad registers and stores two held masks plus two newly-pressed masks:

| WRAM | Portable field |
|---:|---|
| `$0500` | Player 1 held |
| `$0502` | Player 2 held |
| `$0504` | Player 1 newly pressed |
| `$0506` | Player 2 newly pressed |
| `$050E` | Selected active held mask |
| `$0510` | Selected active pressed mask |

New presses use the confirmed expression `(new ^ old) & new`. `$00:C1B2-$00:C20A` selects the active controller from the current player mode and merges both controllers in the two shared-input modes.

## Host callback registry

SNES callback addresses remain stable source identities. A registry maps a confirmed 24-bit source address to a translated C function. Missing callbacks are counted rather than guessed, allowing conversion to proceed incrementally while preserving traceability.

## Object execution

The translated `$BF:8000` scheduler now feeds selected slots into the callback registry. Each invocation receives the frame number, object slot, source address, and the portable object world. Results distinguish executed, missing, and failed callbacks.

## Rendering path

The render queue subtracts the camera origin from object world coordinates, culls against a configurable viewport, and performs stable ordering using the confirmed object attribute priority (`attributes & $003F`). A host tilemap renderer draws wrapped 8x8 indexed tiles into any viewport width; it intentionally contains no original tile graphics.

## Integrated frame step

One native frame now performs:

1. Held/newly-pressed input update.
2. Active-controller selection.
3. Original object scheduling policy.
4. Translation-registry callback invocation.
5. Confirmed PPU scroll transform.
6. Camera-relative visible-object queue generation.

This is a functional runtime path, but not yet a playable Donkey Kong Country scene. Collision, translated gameplay callbacks, real level/tilemap loading, original-compatible asset decoding, audio, saves, and menus remain.
