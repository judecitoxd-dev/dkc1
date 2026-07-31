# Player states 50–70, original frame pieces and NMI VRAM DMA

## Player states 50–70

`player_late_runtime` translates the local mutations in handlers `$BF:99DB-$BF:9E14`. It covers paired-player frame mirroring, position synchronization, timers, scripted swaps, camera exits, state/event wrappers and the reset flow at `$BF:9D5F`.

Every external helper that remains untranslated is returned in a required-call mask. The module does not silently treat those helpers as completed gameplay.

## Original frame layout

The object renderer indexes a four-byte pointer table at `$BB:CC9C` directly with the even frame id. The entry contains a 16-bit address and bank byte. A frame descriptor starts with eight bytes followed by interleaved X/Y piece coordinates.

For frame `$0330`:

```text
pointer: $D7:620A
large pieces: 2
small pieces: 10
auxiliary pieces: 0
total: 12
layout signature: 9A2E475D9D1AB40F
```

Large-piece tile numbers follow the original two-tile stepping and row-wrap rule. Small and auxiliary groups use their header-provided tile offsets and increment one tile per piece. Encoded coordinates are retained, while signed convenience offsets use the renderer's `$80/$70` coordinate biases.

`dk1_object_frame_layout_emit_oam` writes the pieces into the portable OAM model. Its base X/Y arguments correspond to the already transformed renderer bases used before `$BB:AADF`; complete world/camera transformation remains part of the pending original object renderer integration.

## NMI VRAM DMA records

`$81:8CB0-$81:8CE6` configures channel 0 with `$4300=$1801` and consumes eight-byte records beginning at `$170F`:

```text
uint16 transfer length
uint16 VRAM word destination
uint16 source address
uint16 active flag / source bank
```

Bit 15 marks a record active and the low bank byte supplies `$4304`. Processing stops at the first inactive record. After DMA, the original routine clears the active/bank word. The portable queue reproduces this record format, mode-1 linear VRAM write and clearing behavior; it does not claim cycle-accurate DMA timing.
