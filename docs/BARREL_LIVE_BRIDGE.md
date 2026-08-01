# Live common-barrel bridge

`barrel_live_runtime` joins the translated common barrel dispatcher to a portable, deterministic object loop without pretending that unresolved helpers are already exact.

## Supported objects

The bridge accepts the five types whose type table uses `$BF:CF0C`:

- Steel Keg `$22`
- Barrel `$23`
- Rope Barrel `$24`
- DK Barrel `$26`
- TNT Barrel `$27`

Oil Drum `$25` is rejected because it uses the independent callback `$BF:83A0`.

## Runtime phases

```text
IDLE → HELD → THROWN → ROLLING
                    ↘ INACTIVE on break/explosion/release
```

The bridge stores the original `Dk1BarrelObject`, a translated 8.8 motion record, object slot, idle animation ID, terrain attributes and deterministic counters.

## Terrain interface

The module does not invent another map format. It accepts two callbacks:

- `floor_crossing`: reports a floor crossed between the previous and new bottom positions.
- `solid_point`: tests the three leading-side probes used for wall collision.

This permits a future adapter to call `rom_terrain` directly while keeping the focused test synthetic and ROM-independent.

## Dispatcher service

- Held barrels use state 2 with the animation-only carry path.
- Thrown/rolling barrels use state 3.
- Wall impacts are fed to the translated `D23C` branch.
- Floor impacts are fed to the translated `D324` branch.
- Steel Keg survival reverses velocity in the portable helper adapter.
- TNT and DK Barrel destruction results are consumed exactly from `Dk1BarrelRuntimeResult`.

The adapter exposes, rather than hides, the boundary between confirmed dispatcher semantics and portable helper policy.

## Current limitation

This is not yet visible in the X11 frontend. It does not parse a barrel from a level object stream, drive authentic barrel animation frames, merge its OAM/VRAM with the player, or enter the original player pickup/throw state graph. Those connections are required before the public percentage can advance.
