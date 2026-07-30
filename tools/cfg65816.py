#!/usr/bin/env python3
"""Discover direct 65C816 control flow without exporting ROM code bytes.

The output is analysis metadata only: routine entry points, direct call/jump
edges, instruction counts, and unresolved indirect-control-flow sites.
"""
from __future__ import annotations

import argparse
import json
from collections import deque
from dataclasses import dataclass
from pathlib import Path

from dis65816 import OPS, WidthState, hirom_offset, operand_size, parse_pc

CONDITIONAL_BRANCHES = {
    "BCC", "BCS", "BEQ", "BMI", "BNE", "BPL", "BVC", "BVS"
}
RETURN_OPS = {"RTS", "RTL", "RTI", "STP"}
DIRECT_CALLS = {("JSR", "abs"), ("JSL", "long")}
DIRECT_JUMPS = {("JMP", "abs"), ("JML", "long")}
INDIRECT_CONTROL = {
    ("JMP", "absi"), ("JMP", "absix"), ("JML", "absl"),
    ("JSR", "absix"),
}


@dataclass(frozen=True)
class DecodeState:
    pc: int
    m8: bool
    x8: bool


@dataclass(frozen=True)
class Instruction:
    pc: int
    next_pc: int
    mnemonic: str
    mode: str
    raw: bytes
    state_after: WidthState


def decode(rom: bytes, state: DecodeState) -> Instruction:
    off = hirom_offset(state.pc)
    opcode = rom[off]
    mnemonic, mode = OPS[opcode]
    widths = WidthState(m8=state.m8, x8=state.x8)
    size = operand_size(mode, widths)
    raw = rom[off + 1 : off + 1 + size]
    if len(raw) != size:
        raise ValueError(f"instruction at ${state.pc:06X} crosses end of ROM")

    next_pc = (state.pc + 1 + size) & 0xFFFFFF
    if mnemonic == "REP" and raw:
        if raw[0] & 0x20:
            widths.m8 = False
        if raw[0] & 0x10:
            widths.x8 = False
    elif mnemonic == "SEP" and raw:
        if raw[0] & 0x20:
            widths.m8 = True
        if raw[0] & 0x10:
            widths.x8 = True

    return Instruction(
        pc=state.pc,
        next_pc=next_pc,
        mnemonic=mnemonic,
        mode=mode,
        raw=raw,
        state_after=widths,
    )


def direct_target(insn: Instruction) -> int | None:
    value = int.from_bytes(insn.raw, "little") if insn.raw else 0
    bank = insn.pc & 0xFF0000
    if insn.mode == "rel8":
        disp = value if value < 0x80 else value - 0x100
        return (insn.next_pc + disp) & 0xFFFFFF
    if insn.mode == "rel16":
        disp = value if value < 0x8000 else value - 0x10000
        return (insn.next_pc + disp) & 0xFFFFFF
    if insn.mode == "abs":
        return bank | value
    if insn.mode == "long":
        return value
    return None


def discover(rom: bytes, seeds: list[DecodeState], max_instructions: int) -> dict:
    pending = deque(seeds)
    visited: set[DecodeState] = set()
    routine_entries = {seed.pc for seed in seeds}
    edges: set[tuple[int, int, str]] = set()
    unresolved: set[tuple[int, str, str]] = set()
    instruction_addresses: set[int] = set()
    width_variants: dict[int, set[tuple[bool, bool]]] = {}

    while pending and len(visited) < max_instructions:
        state = pending.popleft()
        if state in visited:
            continue

        try:
            insn = decode(rom, state)
        except (IndexError, ValueError):
            unresolved.add((state.pc, "decode", "outside_rom"))
            continue

        visited.add(state)
        instruction_addresses.add(insn.pc)
        width_variants.setdefault(insn.pc, set()).add((state.m8, state.x8))
        next_state = DecodeState(insn.next_pc, insn.state_after.m8, insn.state_after.x8)
        key = (insn.mnemonic, insn.mode)

        if key in DIRECT_CALLS:
            target = direct_target(insn)
            if target is not None:
                edges.add((insn.pc, target, "call"))
                routine_entries.add(target)
                pending.append(DecodeState(target, insn.state_after.m8, insn.state_after.x8))
            pending.append(next_state)
            continue

        if key in DIRECT_JUMPS:
            target = direct_target(insn)
            if target is not None:
                edges.add((insn.pc, target, "jump"))
                pending.append(DecodeState(target, insn.state_after.m8, insn.state_after.x8))
            continue

        if key in INDIRECT_CONTROL:
            unresolved.add((insn.pc, insn.mnemonic, insn.mode))
            if insn.mnemonic == "JSR":
                pending.append(next_state)
            continue

        if insn.mnemonic in CONDITIONAL_BRANCHES:
            target = direct_target(insn)
            if target is not None:
                edges.add((insn.pc, target, "branch"))
                pending.append(DecodeState(target, insn.state_after.m8, insn.state_after.x8))
            pending.append(next_state)
            continue

        if insn.mnemonic in {"BRA", "BRL"}:
            target = direct_target(insn)
            if target is not None:
                edges.add((insn.pc, target, "branch"))
                pending.append(DecodeState(target, insn.state_after.m8, insn.state_after.x8))
            continue

        if insn.mnemonic in RETURN_OPS or insn.mnemonic in {"BRK", "COP"}:
            continue

        pending.append(next_state)

    ambiguous_widths = [
        {
            "address": f"{pc:06X}",
            "states": [
                {"m8": m8, "x8": x8}
                for m8, x8 in sorted(states)
            ],
        }
        for pc, states in sorted(width_variants.items())
        if len(states) > 1
    ]

    return {
        "schema": 1,
        "instruction_state_count": len(visited),
        "unique_instruction_addresses": len(instruction_addresses),
        "routine_entry_count": len(routine_entries),
        "routine_entries": [f"{pc:06X}" for pc in sorted(routine_entries)],
        "edges": [
            {"from": f"{src:06X}", "to": f"{dst:06X}", "kind": kind}
            for src, dst, kind in sorted(edges)
        ],
        "unresolved_control_flow": [
            {"address": f"{pc:06X}", "mnemonic": mnemonic, "mode": mode}
            for pc, mnemonic, mode in sorted(unresolved)
        ],
        "ambiguous_width_states": ambiguous_widths,
        "truncated": bool(pending),
        "instruction_limit": max_instructions,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument(
        "--seed",
        action="append",
        default=[],
        help="24-bit SNES address as BB:AAAA; may be repeated",
    )
    parser.add_argument("--m16", action="store_true")
    parser.add_argument("--x16", action="store_true")
    parser.add_argument("--max-instructions", type=int, default=100000)
    args = parser.parse_args()

    seed_values = args.seed or ["00:8000", "00:A976", "00:A99E"]
    seeds = [
        DecodeState(parse_pc(value), not args.m16, not args.x16)
        for value in seed_values
    ]
    result = discover(args.rom.read_bytes(), seeds, args.max_instructions)
    result["seeds"] = [f"{seed.pc:06X}" for seed in seeds]

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(
        f"discovered {result['routine_entry_count']} routine entries and "
        f"{result['unique_instruction_addresses']} instruction addresses"
    )
    if result["truncated"]:
        print("warning: analysis stopped at the instruction limit")
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
