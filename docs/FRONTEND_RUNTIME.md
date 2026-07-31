# Host frontend, OAM, effects, replay, and saves

This stage adds a functional PC-facing presentation layer around the reconstructed scene state. It does not claim that original player/enemy gameplay callbacks are translated.

## OAM and OBJ

`Dk1OamImage` models the SNES 512-byte low OAM table and 32-byte high table. Each of 128 entries exposes signed 9-bit X, Y, 9-bit tile id, OBJ palette, priority, H/V flip, and small/large selection. The renderer supports every OBSEL size pair, both character-name regions, multi-tile sprites, color-zero transparency, and reverse OAM draw order.

`Dk1OamAnimation` provides a host-side timeline for testing tile changes, durations, offsets, flips, looping, and completion. It is infrastructure for future translated animation scripts, not a decoded original script format yet.

## PPU effects

The effects module provides:

- Mode-1 layer/OBJ priority ranking, including BG3 high-priority mode.
- Two horizontal windows with per-layer enable/invert and OR/AND/XOR/XNOR combination.
- Saturating 5-bit addition/subtraction, optional half result, and brightness scaling.

The current scene compositor still needs these controls connected at exact per-pixel main/subscreen timing for pixel-perfect output.

## Input, replay, and save states

Host keys map to the SNES 16-bit controller mask. Input state tracks held and newly pressed buttons.

A replay records up to 4096 held-button frames and reruns them from a known scene runtime. A stable signature allows regression comparison.

The portable save-state record stores level id, flags, scene-memory signature, camera, viewport, and frame number. It is versioned and checksummed. It is a development save state, not yet original SRAM campaign progress.

## Software and X11 frontends

`Dk1SoftwareFrontend` steps camera/input state, composes reconstructed backgrounds, overlays OAM, and applies display brightness. It includes a movable diagnostic marker so OBJ rendering can be exercised before an original actor callback is translated.

`dk1_frontend_probe` runs scripted frames and writes a PPM. `dk1_frontend_validate` does the same across all 230 scene ids. For the supported ROM, local validation result is:

```text
frontends=230 failed=0 signature=2BA007DBD5D4A725
```

When X11 is available, `dk1_x11` opens a real PC window and presents the same software framebuffer with keyboard input. X11 is optional so the core remains buildable on other platforms and headless CI.
