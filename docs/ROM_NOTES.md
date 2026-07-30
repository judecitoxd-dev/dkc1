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
- Initial CPU entry: `$00:8000`
- SHA-256: `628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527`

## Early boot behavior confirmed

The reset entry immediately transitions the 65C816 into native mode, disables maskable interrupts, selects 16-bit register widths during setup, initializes bank/direct-page/stack state, and enables the SNES fast-ROM setting before continuing into initialization subroutines.

Addresses and behavior are recorded; no original ROM bytes are committed.
