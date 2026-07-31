# Object frame scheduler

The portable scheduler in `src/object_scheduler.c` is a semantic translation of
`$BF:8000-$BF:80F4` plus the secondary-pool loop at `$BF:815E-$BF:8175`.
It selects real callback addresses from the existing 122-entry type table; it
does not generate placeholder object functions.

## Frame order

1. Transform `$1AFB` exactly as `XBA; AND #$FF00` and clear bit `$1000` in `$0579`.
2. Visit every active secondary object, slots `26-57`.
3. Inspect `$1929` and choose one primary-object policy.
4. If `$0535 == 2`, decrement it.
5. Request the shared post-update routine represented by `$BF:C392`.

A negative `$1929` still permits the secondary pool, increments `$1929`, and
skips the primary and post-update portions for that frame.

## Primary policies

| Condition | Portable pass | Selection |
|---|---|---|
| `($1929 & $001A) == 0` | `PRIMARY_NORMAL` | Every active primary slot `1-25`. |
| `($1929 & $0018) == 0` | `PRIMARY_RESTRICTED` | Slots `1-2`, types `$31/$17`, and type `$26` when its `$1029` state is neither `0` nor `5`. |
| Bit `$0010` path | `PRIMARY_FOCUS` | The focus offset from `$1923`, plus every type `$45`. |
| Bit `$0008` path | `PRIMARY_SPECIAL` | Non-focus objects whose type record has attribute bit `$0080`. |

Each selected type id is converted into its confirmed bank-`$BF` handler from
`$BF:817C`. Host code receives the slot, pass kind, and 24-bit SNES callback
address, allowing translated C handlers to replace addresses incrementally.

## First scroll transform

`dk1_ppu_scroll_half_parallax()` translates `$80:8973-$80:89B3`:

- BG1 X/Y follows camera `$088B/$0895`.
- BG2 X/Y uses camera divided by two.
- BG3 vertical scroll is set to zero.

This is the first confirmed per-frame parallax output that can be consumed by a
native PC renderer. Original graphics and level data are not stored here.
