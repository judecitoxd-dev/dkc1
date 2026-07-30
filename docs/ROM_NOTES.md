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

The reset entry transitions the 65C816 into native mode, disables maskable interrupts, establishes direct-page/data-bank/stack state, enables FastROM, and enters the first memory-initialization routine.

The first confirmed DMA operations are now represented semantically in C:

- `$00:A8E6` performs two fixed-source 64 KiB DMA passes through the WRAM data port, clearing the full 128 KiB WRAM address space.
- `$00:A8C2` performs one fixed-source 64 KiB DMA pass to the VRAM data ports, clearing 32 KiWords / 64 KiB of VRAM.
- `$00:A920` is the callable WRAM-bank-1 clear path used during later reset.

Addresses and behavior are recorded; no original ROM bytes are committed.
