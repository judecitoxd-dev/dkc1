# Player states 71–86, screen OAM, frame DMA and SPC driver startup

## Final player-state range

`player_final_runtime` translates the local mutations in `$BF:9DE0-$BF:A066`. It covers states 71–86: countdowns, gravity, movement wrappers, collision propagation, exit snapshot/restore, event/input wrappers, blink-frame logic, linked-object initialization and animation-only tails.

The module models only fields touched locally. External helpers are returned in a required-call mask, and post-input state can be supplied explicitly before the two blink handlers continue.

## Screen-coordinate preparation

The object renderer computes a screen center from world coordinates and scroll registers before expanding frame pieces. The portable path uses:

```text
center_x = world_x - camera_x
center_y = vertical_origin - world_y - vertical_scroll + vertical_offset
```

For an unflipped piece, its signed descriptor offset is added to this center. Mirrored pieces subtract the signed offset and their actual sprite width/height, matching the `$FF` coordinate inversion and `$8F/$80` base biases in the original renderer.

## Frame graphics DMA producer

After the renderer consumes the descriptor coordinates, the remaining frame data is graphics payload. `$BB:A9A3` schedules up to two records:

```text
primary length = attribute_5 * 32
primary destination = (tile_base & $01FF) * 16 words
secondary length = (attribute_7 & $0F) * 32
secondary destination = primary + attribute_6 * 16 words
```

For frame `$0330`, graphics begin at `$D7:622A`; the two lengths are 448 and 128 bytes.

## SPC driver startup

`$CA:B133` copies `$0D7B` bytes from `$8A:A36E` to SPC address `$04E8`, then uses the normal upload-block routine on `$C9:2D95`, whose header specifies length `$0E26` and destination `$2380`.

The driver launch address used after loading song data is `$05E8`. The bounded trace currently supports only the initial proven opcodes: NOP, CLRP, immediate X load, SP transfer, direct/absolute A moves and CALL. It reaches `$1076` after 11 instructions and stops on opcode `$BE` rather than guessing decimal-adjust/BRK initialization behavior.
