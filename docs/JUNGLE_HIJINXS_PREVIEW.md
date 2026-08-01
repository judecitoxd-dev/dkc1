# Jungle Hijinxs provisional preview milestone

This is the next user-facing delivery target. It is intentionally smaller than
an original-compatible full-game port, but it must be coherent enough to play
from the Jungle Hijinxs entrance to a provisional exit in a native Windows
build.

## Required before publishing the user test build

- Native Windows x64 executable that asks for the user's own unheadered USA Rev
  2 ROM. No ROM or extracted assets are packaged.
- Player can traverse the source terrain from the entrance to the far end without
  debug teleporting.
- Camera follows and renders the complete route at 384x224 internally, scaled to
  the window while preserving aspect ratio.
- Multiple simultaneously visible Gnawty records are supported, including stomp,
  side-contact knockback, invulnerability and defeated-record persistence.
- Source Barrel can be picked up, carried and thrown without stopping the frame
  loop.
- A provisional end trigger confirms that the route was completed.
- Any known blocking crash or deterministic frame failure must be fixed.
- Windows build artifact must pass its GitHub Actions compile/package job.

## Allowed provisional differences in the first test

- The end trigger may use the level's maximum camera/player coordinate instead of
  the original exit object and celebration sequence.
- Collectibles may initially be non-interactive or omitted from the HUD.
- Only Gnawty and the source Barrel are required as interactive objects in the
  first public test; other enemies remain a later fidelity pass.
- Player damage may use the documented portable knockback/invulnerability bridge.
- Music and sound effects may be silent.
- Donkey/Diddy switching, lives, bonus rooms and SRAM are not required for this
  first route test.

## Not a definition of the final port

Reaching this milestone means the first level route can be tested and feedback
can be collected. It does not mean Jungle Hijinxs is yet a faithful replacement
for the original level, nor that the rest of the game is complete.
