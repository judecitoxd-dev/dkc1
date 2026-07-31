# Player states 35–49, linked animation and SPC upload blocks

## Advanced player states

`player_advanced_runtime` translates the local portions of handlers `$BF:93FD-$BF:9999`, corresponding to states 35–49. It models the player, partner/linked object and second player fields actually touched by those routines. Unknown calls remain in a 64-bit required-call mask.

State 35 performs the source-relative fixed-point movement used by its scripted sequence. States 37/38 validate linked types `$30/$31`. State 40 contains its mode-dependent timer and partner flow. State 44 includes the helper at `$BF:97C8`. State 49 implements the local off-camera reset of both players.

## Reverse and linked animation

Positive animation speed follows `$BE:80E1`; negative speed follows `$BE:8414`. The interpreter now supports all dispatch-table commands `$80-$91`.

Commands `$85-$88` use the owner `$0512` link, `$89-$8C` use `$16F5`, and `$8D` uses the direct object link stored in `$1375`. The caller resolves those indices into portable linked-object records before stepping the script.

## Audio catalog and upload blocks

The CPU routine at `$8A:B1CB` indexes 27 packed 24-bit source addresses beginning at `$8A:B15E`. Each source begins with the header consumed by `$8A:B4EB`:

```text
uint16 little-endian payload length
uint16 little-endian SPC destination
payload bytes
```

The transfer routine advances the SPC destination for every byte and sends data through APUIO1 while APUIO2/3 hold the target address. The clean-room loader reproduces the resulting SPC RAM image without emulating transfer timing.
