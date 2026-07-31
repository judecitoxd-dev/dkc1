# Verified ROM notes

The user-provided dump was inspected locally; the ROM itself is not stored here.

- Size: `0x400000` bytes (4 MiB)
- Mapping: HiROM
- Internal title: `DONKEY KONG COUNTRY`
- Map mode: `0x31`
- Cartridge type: `0x02`
- ROM size code: `0x0C`
- SRAM size code: `0x01`
- Region code: `0x01`
- Internal version: `2`
- Header offset: `0x00FFC0`
- Emulation reset vector: `$8000`
- Native NMI vector: `$A976`
- Native IRQ vector: `$A99E`
- Initial CPU entry: `$00:8000`
- SHA-256: `628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527`

## Early boot behavior confirmed

The reset entry transitions the 65C816 into native mode, disables maskable
interrupts, establishes direct-page/data-bank/stack state, enables FastROM, and
enters the first memory-initialization routine.

Confirmed DMA behavior represented semantically in C:

- `$00:A8E6` clears all 128 KiB of WRAM with two fixed-source 64 KiB DMA passes.
- `$00:A8C2` clears 64 KiB of VRAM with one fixed-source DMA pass.
- `$00:A920` is the callable WRAM-bank-1 clear path used during later reset.

Confirmed scheduler and boot-state behavior:

- `$80:A97A` dispatches NMI through the 16-bit pointer at `$001C`.
- `$80:80A9` is the main per-frame NMI callback and resets SP to `$01FF`.
- `$0508` stores the active state callback.
- `$050A` stores the state-sequence table base.
- `$050C` stores the sequence element index.
- `$00:BA43-$00:BA8D` establishes initial control variables and request IDs.
- `$00:BA91-$00:BAEF` establishes the first verified PPU configuration and VRAM loops.

## Level and object dispatch behavior confirmed

- `$003E` is used as a 230-value level/location dispatch identifier.
- `$80:C56C` stores paired bank-$80 callbacks selected by `$80:9723`.
- `$BF:FDC8` stores bank-$80 entry callbacks selected by `$80:86E0`.
- `$B9:801E`, `$B9:81EA`, and `$B9:83B6` are parallel 230-word dispatch tables.
- `$BF:817C` is a 122-entry object-type record table with a handler word and metadata word.
- `$130D/$1341` form a dynamic 24-bit per-object callback.
- `$BF:8000` updates primary slots `1-25`; `$BF:815E` updates secondary slots `26-57`.
- `$0D45` is the parallel object type/active array.
- `$0B19/$0BC1` are object world X/Y arrays.
- `$116D` is the object script pointer array.
- `$81:D931` and `$BE:812E` dispatch the address in `$1341,X` using the low byte of `$130D,X` as the bank.
- `$BE:8197` is the confirmed script operation that writes the callback address and bank.

## Camera behavior confirmed

- `$088B` is camera X and `$0895` is camera Y.
- Rendering paths subtract those values from object world positions.
- `$1B23/$1B25` are the horizontal camera minimum and maximum.
- `$80:9C9D-$80:9CDD` clamps X to those limits and normally clamps Y to `0..$0200`.
- Mode `$0009` bypasses the upper Y clamp.
- `$0889/$0897` are auxiliary camera values cleared whenever the corresponding axis is clamped.

Addresses and behavior are recorded; no original ROM or asset bytes are committed.
