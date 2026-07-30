# Level dispatch map

The direct-page word at `$003E` is used as a level/location identifier in several confirmed dispatch paths.

## Bank `$80` frame dispatch

`$80:9723-$80:972F` multiplies `$003E` by four and uses one four-byte entry at `$80:C56C`:

1. a 16-bit callback called through `JSR ($C56C,X)`;
2. a 16-bit callback reached through `JMP ($C56E,X)`.

The table contains **230 entries** before executable code resumes. Its first column has 21 unique callback addresses and its second column has 18.

## Bank `$80` entry dispatch

`$80:86C5-$80:86E0` selects a 16-bit callback from `$BF:FDC8`, supplies program bank `$80`, and performs `JML [$007A]`. The 230 confirmed entries use 11 unique destinations.

## Bank `$B9` level tables

The three entry points `$B9:8000`, `$B9:8010`, and `$B9:8017` each index a 230-word table using `$003E`:

- `$B9:801E` — 31 unique destinations;
- `$B9:81EA` — 23 unique destinations;
- `$B9:83B6` — 19 unique destinations.

The portable C module `level_dispatch.c` preserves only these callback addresses and table relationships. It contains no graphics, level geometry, music, or ROM instruction bytes.
