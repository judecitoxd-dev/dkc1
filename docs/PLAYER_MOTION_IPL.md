# Fixed-point player motion, host preview control and clean-room IPL protocol

## Shared player motion helpers

`player_motion` translates four confirmed shared routines from bank `$BF`:

- `$AF81`: owner-selected/default gravity with the original `-$0800` terminal velocity.
- `$AFB2`: light gravity with `-$0140` or `-$0200` terminal velocity.
- `$AFE4`: horizontal signed 8.8 integration into `$0B19/$0DB9`.
- `$B012`: vertical signed 8.8 integration into `$0BC1/$0E21`.

The response table at `$BF:B29E` is also represented. Modes 0–7 shift a velocity difference by 3, 4, 5, 6, 7, 8, 2 or 1 bits; mode 8 returns `difference*3/32`. `dk1_player_motion_approach_horizontal` reproduces the local target-velocity approach at `$BF:B1D5` without overshooting.

## Host preview runtime

`player_preview_runtime` uses those exact helpers for the interactive preview:

- L/R select a target velocity of `-$0180/$0180`.
- The confirmed response curve approaches that target.
- B applies a preview jump impulse and gravity then runs the same fixed-point integrators.
- Facing is propagated to the authentic frame OAM transform.

The landing plane is currently the preview's initial Y position. This is intentionally not described as original level collision; ROM terrain contacts and material side effects remain a separate integration task.

## Clean-room IPL behavior

`spc700_ipl_protocol` models the public CPU/SPC handshake behavior after the startup driver's BRK handoff without embedding Nintendo's 64-byte IPL ROM. It exposes the `$AA/$BB` ready signature, accepts command `$CC`, transfers token-acknowledged bytes to SPC RAM and launches a caller-selected entry point.

The gameplay validator uploads three bytes at `$3000` and relaunches the already loaded driver at `$05E8`. This validates the bridge beyond `$FFC0`; it does not emulate IPL instruction timing or produce DSP audio.
