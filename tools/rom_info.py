#!/usr/bin/env python3
"""Validate and describe a headerless SNES HiROM image without extracting it."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

EXPECTED_SHA256 = "628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527"
HEADER_OFFSET = 0xFFC0


def u16le(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset : offset + 2], "little")


def inspect(path: Path) -> dict[str, object]:
    rom = path.read_bytes()
    if len(rom) < 0x10000:
        raise ValueError("file is too small to contain a standard SNES header")

    header = rom[HEADER_OFFSET : HEADER_OFFSET + 0x40]
    title = header[:21].decode("ascii", errors="replace").rstrip()
    digest = hashlib.sha256(rom).hexdigest()
    checksum = u16le(header, 0x1E)
    complement = u16le(header, 0x1C)

    return {
        "path": str(path),
        "size_bytes": len(rom),
        "sha256": digest,
        "expected_revision": digest == EXPECTED_SHA256,
        "title": title,
        "mapping": "HiROM" if (header[0x15] & 1) else "LoROM",
        "map_mode": f"0x{header[0x15]:02X}",
        "cartridge_type": f"0x{header[0x16]:02X}",
        "rom_size_code": f"0x{header[0x17]:02X}",
        "sram_size_code": f"0x{header[0x18]:02X}",
        "region_code": f"0x{header[0x19]:02X}",
        "version": header[0x1B],
        "checksum": f"0x{checksum:04X}",
        "checksum_complement": f"0x{complement:04X}",
        "checksum_pair_valid": ((checksum + complement) & 0xFFFF) == 0xFFFF,
        "reset_vector": f"0x{u16le(rom, 0xFFFC):04X}",
        "nmi_vector": f"0x{u16le(rom, 0xFFFA):04X}",
        "irq_brk_vector": f"0x{u16le(rom, 0xFFFE):04X}",
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--strict", action="store_true", help="fail unless SHA-256 matches Rev 2")
    args = parser.parse_args()

    try:
        result = inspect(args.rom)
    except (OSError, ValueError) as exc:
        parser.error(str(exc))

    print(json.dumps(result, indent=2))
    if args.strict and not result["expected_revision"]:
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
