# Local ROM, graphics, and terrain pipeline

No ROM bytes or generated images are committed. All decoding operates on the user's local cartridge dump.

## Supported cartridge identity

The C runtime checks the 4 MiB USA Rev. 2 HiROM header:

- Internal title: `DONKEY KONG COUNTRY`
- Map mode: `$31`
- Cartridge type: `$02`
- ROM size code: `$0C`
- SRAM size code: `$01`
- Region: `$01`
- Version: `2`
- Checksum plus complement: `$FFFF`

The loader provides checked conversion from 24-bit SNES addresses to ROM offsets and checked little-endian reads.

## Graphics

The standard SNES 4bpp decoder converts each 32-byte planar tile into 64 palette indices. The palette decoder converts 15-bit SNES colors into RGBA8. These routines can consume any uncompressed local-ROM range; compressed DKC resources require their specific decompressor before tile decoding.

`dk1_asset_probe` accepts a ROM path, tile address, palette address, and output PPM path. It decodes 16 tiles into a 4x4 preview. Addresses are supplied explicitly because not every ROM range contains raw graphics.

## Terrain query

`$81:8000` loads object world X from `$0B19,X` and dispatches through the level terrain callback at `$1B0F`. The main callback at `$81:800D` uses:

- A map pointer in `$D3`.
- A block-half descriptor pointer in `$D7`.
- A block-count bound in `$DB`.
- Object Y from `$0BC1,X`.

The reconstructed map has 16 vertical rows covering 512 pixels. Rows are reversed in memory and columns are stored consecutively. Each 32x32 map cell selects a block containing two 16-pixel collision descriptors. Cell bit `$4000` mirrors the full block. Descriptor bit `$0080` mirrors one half.

The descriptor's lower six bits select a height transform through the pointer table at `$81:84C9`. Sixty-three valid transforms have been translated to C: constant surfaces, quarter/half/full/double/quadruple slopes, a small curve lookup at `$81:86E5`, and their offsets/clamps. Shape zero means no collision; shape `$3F` is treated as unsupported rather than executed as code.

The current portable terrain module decodes exact individual cells and searches downward for a floor. The remaining work is to bind the level-specific pointer records selected at `$81:8C67-$81:8CAF` and reproduce every neighboring-cell/material side effect of the complete original query.

## Validation actor

The preview actor exists to test input and collision integration. It applies host-side acceleration, gravity, and jump values, then lands against the reconstructed terrain surface. Those constants are not claimed to be original player physics.
