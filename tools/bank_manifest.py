#!/usr/bin/env python3
"""Create a non-copyrightable per-bank hash/statistics manifest for a SNES ROM."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

BANK_SIZE = 0x10000


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    rom = args.rom.read_bytes()
    if len(rom) % BANK_SIZE:
        parser.error("ROM size is not a whole number of 64 KiB HiROM banks")

    banks = []
    for offset in range(0, len(rom), BANK_SIZE):
        chunk = rom[offset : offset + BANK_SIZE]
        banks.append(
            {
                "bank": f"{offset // BANK_SIZE:02X}",
                "offset": f"0x{offset:06X}",
                "size": len(chunk),
                "sha256": hashlib.sha256(chunk).hexdigest(),
                "nonzero_bytes": sum(value != 0 for value in chunk),
                "ff_bytes": chunk.count(0xFF),
            }
        )

    output = {
        "rom_sha256": hashlib.sha256(rom).hexdigest(),
        "size_bytes": len(rom),
        "bank_size": BANK_SIZE,
        "banks": banks,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {len(banks)} banks to {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
