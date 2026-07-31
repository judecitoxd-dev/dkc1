# Level profile, PPU preset, and VRAM package pipeline

No ROM, decompressed graphics, palette data, or generated tile sheets are committed. All cartridge reads happen from the user's local USA Rev. 2 dump.

## Level id to terrain profile

The bank-`$B9` initialization callback selected for each of the 230 values of level/location id `$003E` was followed through local `JSR`/`JSL` control flow. Every callback reaches `$81:8C66` with exactly one immediate profile id in the range `0..13`.

The portable table keeps both the original bank-`$B9` callback address and the resolved terrain/streaming profile. All 230 ids resolve, and all 14 profiles are used.

## PPU presets

`$B9:A4DC` selects one of 23 compact register streams beginning at `$B9:A50E`. Each command contains a PPU register address and either one value or two values for consecutive registers. The high bit of the encoded address selects the two-byte form; a zero address terminates the preset.

The portable parser reconstructs the written `$2100-$213F` register values. This exposes the actual BG mode, tilemap bases, character-data bases, color math, and layer-enable state used by each scene initializer.

## VRAM packages

`$B9:A924` selects one of 30 package lists beginning at `$B9:A994`. The supported ROM contains 96 seven-byte DMA records:

1. Source bank.
2. Source address.
3. VRAM word destination.
4. Transfer byte count.

Destination bit `$8000` marks a compressed source. Direct records may read from ROM or from WRAM banks `$7E/$7F`.

## Compression format

Compressed records call `$B8:982F`. A 128-byte dictionary precedes the command stream; commands begin at source address + `$80` and write into scratch WRAM `$7E:79FC`.

| Control bits | Behavior |
|---|---|
| `00nnnnnn` | Copy `n` literal bytes from the stream. |
| `01nnnnnn` | Repeat the following byte `n` times. |
| `10nnnnnn` | Copy `n` bytes from an absolute offset in the already-produced output; the offset follows as a little-endian word. |
| `11iiiiii` | Copy dictionary word `i` from the 128-byte prefix. |
| `$00` | End of stream. |

The translated decoder preserves overlapping output copies, bank-local source wrapping, output bounds, and command statistics. Real-ROM validation found 16 compressed package records.

## VRAM reconstruction

The portable VRAM loader owns:

- 64 KiB of VRAM.
- 128 KiB of WRAM for RAM-sourced DMA records.
- A 32 KiB scratch area matching the decompression/DMA path.

It applies direct and compressed package records in original order. Bytes beyond the decompressed prefix retain scratch contents, matching the original behavior before the requested DMA byte count is copied.

## Tile-sheet probe

`dk1_vram_probe` applies one package and renders 4bpp tiles from reconstructed VRAM with a user-selected local-ROM palette:

```bash
./build/dk1_vram_probe \
  "rom/Donkey Kong Country (USA) (Rev 2).sfc" \
  1 0xB9A1DC 0x3000 0 256 package1.ppm
```

For the supported ROM, package 1 contains four records: one compressed record and three direct records. The tool is diagnostic and does not commit its generated image.
