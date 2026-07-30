#!/usr/bin/env python3
from __future__ import annotations

import sys
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS))

from cfg65816 import DecodeState, discover, load_indirect_map  # noqa: E402


class CfgTests(unittest.TestCase):
    def test_direct_call_and_branch_discovery(self) -> None:
        rom = bytearray([0xFF] * 0x10000)
        # $00:8000: REP #$30; JSR $8010; BNE $800A; RTS
        rom[0x8000:0x8009] = bytes([
            0xC2, 0x30,
            0x20, 0x10, 0x80,
            0xD0, 0x03,
            0x60,
            0xEA,
        ])
        rom[0x800A] = 0x60
        rom[0x8010] = 0x60

        result = discover(bytes(rom), [DecodeState(0x008000, True, True)], 100)
        self.assertFalse(result["truncated"])
        self.assertIn("008010", result["routine_entries"])
        self.assertIn(
            {"from": "008002", "to": "008010", "kind": "call"},
            result["edges"],
        )
        self.assertIn(
            {"from": "008005", "to": "00800A", "kind": "branch"},
            result["edges"],
        )

    def test_indirect_jump_is_reported(self) -> None:
        rom = bytearray([0xFF] * 0x10000)
        rom[0x8000:0x8003] = bytes([0x6C, 0x00, 0x20])
        result = discover(bytes(rom), [DecodeState(0x008000, True, True)], 10)
        self.assertEqual(
            result["unresolved_control_flow"],
            [{"address": "008000", "mnemonic": "JMP", "mode": "absi"}],
        )

    def test_resolved_indirect_targets_are_followed(self) -> None:
        rom = bytearray([0xFF] * 0x10000)
        rom[0x8000:0x8003] = bytes([0x6C, 0x00, 0x20])
        rom[0x8010] = 0x60
        rom[0x8020] = 0x60
        result = discover(
            bytes(rom),
            [DecodeState(0x008000, True, True)],
            20,
            {0x008000: [0x008010, 0x008020]},
        )
        self.assertEqual(result["unresolved_control_flow"], [])
        self.assertEqual(
            result["resolved_indirect_control_flow"],
            [
                {"address": "008000", "target": "008010"},
                {"address": "008000", "target": "008020"},
            ],
        )
        self.assertIn("008010", result["routine_entries"])
        self.assertIn("008020", result["routine_entries"])

    def test_load_indirect_map(self) -> None:
        import json
        import tempfile

        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "map.json"
            path.write_text(json.dumps({"sites": {"80:8000": ["80:8010", "81:9000"]}}))
            result = load_indirect_map(path)
        self.assertEqual(result, {0x808000: [0x808010, 0x819000]})


if __name__ == "__main__":
    unittest.main()
