# Object runtime and camera notes

## Object slots

The main object update path at `$BF:8000` uses the 16-bit type-id array beginning at WRAM `$0D45`. Parallel object fields use the same even byte offset, so portable code represents a slot as `offset = slot * 2`.

Two update pools are confirmed:

| Pool | SNES X offsets | Portable slots | Count |
|---|---:|---:|---:|
| Primary | `$0002-$0032` | `1-25` | 25 |
| Secondary | `$0034-$0072` | `26-57` | 32 |

Slot zero is outside those two loops and remains reserved until its separate behavior is fully named.

Confirmed parallel arrays include:

| WRAM base | Meaning |
|---:|---|
| `$0B19` | Object world X position |
| `$0BC1` | Object world Y position |
| `$0C69` | Object flags used by rendering and direction logic |
| `$0D45` | Object type id / active marker |
| `$116D` | Current object script pointer |
| `$130D` | Callback bank field; low byte supplies the JML bank |
| `$1341` | Callback address |

At `$81:D931-$81:D93E` and its bank-`$BE` counterpart, the engine builds a 24-bit callback from `$1341,X` plus the low byte of `$130D,X`, then performs a long indirect jump. The script command at `$BE:8197` writes this address-and-bank pair.

## Type dispatch

The four-byte records beginning at `$BF:817C` cover 122 type ids. Each record contains:

1. A 16-bit callback address in bank `$BF`.
2. A raw 16-bit attribute field.

The update code uses `attributes & $003F` as an ordering value and tests attribute bit `$0080` for a special update pass. Other bits remain raw until their behavior is confirmed.

## Camera

Object rendering subtracts `$088B` from object X and `$0895` from object Y, confirming them as the camera origin.

The clamp routine at `$80:9C9D-$80:9CDD` establishes:

- `$1B23`: horizontal minimum.
- `$1B25`: horizontal maximum.
- `$0889`: auxiliary horizontal value cleared when X is clamped.
- `$0897`: auxiliary vertical value cleared when Y is clamped.
- Normal vertical range: `0..$0200`.
- Game mode `$0009` bypasses the upper vertical clamp.

The portable camera module keeps the original camera origin separate from viewport width and height. This is important for widescreen: the original simulation can remain compatible while the host renderer requests a wider visible rectangle.

No ROM bytes, graphics, level geometry, or complete assembly listing are stored in the repository.
