# Extended player states, animation scripts and executable SPC bootstrap

## Player states 20–34

State 20 is retained as an exact call plan because its linked-object interaction depends on helpers that are not fully translated. States 21–34 have local executable behavior. Every external call remains represented by a required-call flag.

The translated group includes guard/input wrappers, field clearing, scripted launch setup, camera-relative transitions, facing-dependent horizontal velocity, animation-preserving movement and vertical acceleration clamped to `-$0500`.

## Animation interpreter

The original frame path begins at `$BE:80E1`. `$116D` points into bank `$BE`; the routine subtracts `$1139` from `$1105`, consumes records while the accumulator remains negative, writes a selected frame to `$0D11`, and advances the script pointer.

Implemented commands:

- `$80/$91`: reload the script root from `$BE:8572` using `$10D1`.
- `$81`: expose a 24-bit external call and continuation pointer.
- `$82`: jump to another bank-$BE script pointer.
- `$83`: expose a local bank-$BE call.
- `$84`: install the object callback in `$1341/$130D`.
- `$8E-$90`: expose one-byte event commands.

Paired-object commands `$85-$8D` remain unsupported rather than guessed.

## SPC700 bootstrap execution

The 40-byte image uploaded to `$04B8` is executed by a bounded SPC700 subset. The implementation supports only the opcodes used by that image. It models the port bytes at `$F4-$F7`, the token at `$04DE`, indirect upload through the pointer in `$F6/$F7`, acknowledgement through `$F4`, and the final indexed indirect jump.

The current scope proves CPU-to-SPC transfer and launch behavior. It does not execute the larger sound driver or emulate the SNES DSP.
