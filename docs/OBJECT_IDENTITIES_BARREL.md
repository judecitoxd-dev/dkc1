# Confirmed object identities and common barrel dispatcher

## Naming boundary

The type IDs below are confirmed against the Rev 2 type-dispatch table and
corroborated by the public DKC1 disassembly's symbolic catalog. The portable
implementation remains clean-room: no original source or game assets are
copied into this repository.

| Type | Confirmed identity | Callback | Idle animation |
|---:|---|---:|---:|
| `$22` | Steel Keg | `$BF:CF0C` | `$00D1` |
| `$23` | Barrel | `$BF:CF0C` | `$00D2` |
| `$24` | Rope Barrel | `$BF:CF0C` | `$00D3` |
| `$25` | Oil Drum | `$BF:83A0` | `$00D4` |
| `$26` | DK Barrel | `$BF:CF0C` | `$00D5` |
| `$27` | TNT Barrel | `$BF:CF0C` | `$00D6` |
| `$73` | Sign | `$BF:8453` | scene-selected |

The former generic type-`$73` actor is therefore documented as the Sign. Its
old touch-deactivate option remains only as a portable test policy and is no
longer used by supported-ROM gameplay validation.

## Common barrel dispatcher

`barrel_runtime` translates the local semantics of all ten states selected by
the `$BF:CF0C` state table:

```text
0 → $BF:CF2B    5 → $BF:D156
1 → $BF:CF38    6 → $BF:D185
2 → $BF:CF3F    7 → $BF:D198
3 → $BF:CFCC    8 → $BF:D1C7
4 → $BF:D117    9 → $BF:D1DD
```

The runtime covers initialization, animation/render paths, held/thrown
transitions, Steel Keg cleanup, DK Barrel release requests, TNT countdown and
explosion requests, break sounds, spawned explosion scripts, and the reciprocal
link check used by Manky Kong's barrel. Untranslated helpers remain explicit
in a 64-bit required-call mask rather than being guessed.

The Oil Drum is intentionally excluded because its type-table callback is
`$BF:83A0`, not the common `$BF:CF0C` barrel dispatcher.

## Completeness boundary

This is a dispatcher translation and deterministic behavior model. A fully
playable barrel still requires the unresolved collision, ownership, spawning,
player pickup/throw, sound, and level-object helpers to be connected to the
live frontend. For that reason the public project percentage remains 98%.
