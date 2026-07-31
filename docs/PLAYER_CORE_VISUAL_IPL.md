# Core player states, live visual pipeline and SPC IPL handoff

## Player states 2–5 and 10

`player_core_runtime` covers the five previously unclassified player handlers. It models the current player, the partner, and the owner fields touched locally by `$BF:898F-$BF:8FA6`.

The implementation preserves local thresholds and mutations while returning every unresolved call in a required-call mask. In particular, the state-3 horizontal launch magnitude still depends on the original multiplication context; callers may provide the resolved magnitude, and the runtime does not invent it.

Completing these five handlers makes every one of the 87 dispatcher entries measurable. “Local semantics” means the instructions inside that handler are represented; it does not mean every shared helper invoked by the handler is complete.

## Authentic visual composition

`player_visual_runtime` combines previously independent confirmed modules:

```text
frame id
→ bank-$BB frame pointer
→ descriptor and piece layout
→ world/camera screen transform
→ OAM entries
→ frame graphics DMA records
→ private VRAM image
```

The frontend keeps scene VRAM immutable. It copies the scene image, executes the frame DMA into `player_vram`, and uses that image for OBJ rendering. This avoids contaminating shared scene bootstrap data and makes the visual output deterministic.

Frame `$0330` remains the initial diagnostic player frame. Its use does not yet imply original animation selection, palette selection or gameplay physics for every level.

## SPC700 handoff

The full startup image calls `$1076`, whose bytes are `$BE,$0F`: `DAS A` followed by `BRK`. The bounded tracer now implements those instructions, pushes the BRK frame and reports a handoff to `$FFC0`.

The 64-byte SPC IPL ROM is not included. Continuing from `$FFC0` will require either a clean-room behavioral model of the relevant handshake or a user-supplied/reference execution path. The current result proves the driver's startup reaches the IPL handoff; it does not produce audio.
