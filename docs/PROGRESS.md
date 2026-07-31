# Progress accounting

## Current total: 26%

The total is milestone-weighted so it cannot be inflated by generated stubs.

| Milestone | Weight | Done | Contribution |
|---|---:|---:|---:|
| Reproducible ROM identification and mapping | 5% | 100% | 5.00% |
| Function discovery, control-flow graph, symbols | 15% | 55% | 8.25% |
| Portable gameplay/system C | 45% | 43% | 19.35% |
| Graphics, camera, tilemaps, widescreen | 15% | 45% | 6.75% |
| Audio | 8% | 0% | 0.00% |
| Input, saves, menus, compatibility | 7% | 25% | 1.75% |
| Validation and packaging | 5% | 30% | 1.50% |

Weighted pipeline foundation: **42.60%**. The public headline remains **26%** because a complete original-compatible level is still not playable.

## Completed in this five-point stage

### 22% — exact level-id to terrain-profile map

- Followed all 230 bank-`$B9` level-init callbacks selected by level id `$003E`.
- Every id resolves to exactly one of the 14 terrain/streaming profiles consumed by `$81:8C66`.
- Preserved the original bank-`$B9` callback address alongside the resolved profile.
- Confirmed all 14 profiles are used.

### 23% — PPU preset decoding

- Translated the compact register-stream format consumed by `$B9:A4DC`.
- Parses all 23 presets from `$B9:A50E`.
- Reconstructs single-register and consecutive two-register writes in `$2100-$213F`.
- Exposes actual BG mode, screen bases, character bases, layer enables, and color-math registers.

### 24% — VRAM package catalog

- Translated the package-list format consumed by `$B9:A924`.
- Parsed 30 package ids, 96 seven-byte DMA records, ROM sources, WRAM `$7E/$7F` sources, VRAM destinations, byte counts, and compression flags.

### 25% — graphics decompressor

- Translated `$B8:982F-$B8:98C9` into bounded portable C.
- Implemented literal, repeated-byte, output-copy, and 64-word dictionary commands.
- Preserved overlapping copies, bank-local source wrapping, and the `$00` terminator.
- Validated all 16 compressed package records in the supported ROM.

### 26% — reconstructed VRAM and authentic tile sheet

- Added 64 KiB VRAM, 128 KiB WRAM, and 32 KiB decompression scratch models.
- Applies direct and compressed DMA records in original order.
- Added `dk1_vram_probe` to render reconstructed 4bpp tiles with a palette read from the local ROM.
- Locally reconstructed package 1: four records, 34,256 DMA bytes, and 27,968 decompressed bytes.
- Expanded automated validation from 28 to 33 configured tests; the five new tests pass locally with warnings treated as errors.

## Next measurable targets

- Extract exact PPU preset, VRAM package, and palette-upload sequences for every level id.
- Reconstruct the complete CGRAM state rather than supplying one palette address manually.
- Bind reconstructed VRAM tiles to the expanded level map and produce the first correctly composed original background section.
- Complete neighboring-cell/material side effects of `$81:800D`.
- Translate one common original actor callback and its animation data.
