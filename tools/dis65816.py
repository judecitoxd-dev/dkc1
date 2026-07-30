#!/usr/bin/env python3
"""Small state-aware 65C816 linear disassembler for clean-room analysis."""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

# Addressing modes. Immediate width follows M (accumulator) or X (index).
MODES = {
    "imp": 0, "acc": 0, "imm8": 1, "immM": -1, "immX": -2,
    "dp": 1, "dpx": 1, "dpy": 1, "dpi": 1, "dpix": 1, "dpiy": 1,
    "dpl": 1, "dply": 1, "sr": 1, "sriy": 1, "rel8": 1,
    "abs": 2, "absx": 2, "absy": 2, "absi": 2, "absix": 2,
    "absl": 2, "rel16": 2, "block": 2, "long": 3, "longx": 3,
}

# Complete opcode matrix: mnemonic, addressing mode.
ROWS = [
[("BRK","imm8"),("ORA","dpix"),("COP","imm8"),("ORA","sr"),("TSB","dp"),("ORA","dp"),("ASL","dp"),("ORA","dpl"),("PHP","imp"),("ORA","immM"),("ASL","acc"),("PHD","imp"),("TSB","abs"),("ORA","abs"),("ASL","abs"),("ORA","long")],
[("BPL","rel8"),("ORA","dpiy"),("ORA","dpi"),("ORA","sriy"),("TRB","dp"),("ORA","dpx"),("ASL","dpx"),("ORA","dply"),("CLC","imp"),("ORA","absy"),("INC","acc"),("TCS","imp"),("TRB","abs"),("ORA","absx"),("ASL","absx"),("ORA","longx")],
[("JSR","abs"),("AND","dpix"),("JSL","long"),("AND","sr"),("BIT","dp"),("AND","dp"),("ROL","dp"),("AND","dpl"),("PLP","imp"),("AND","immM"),("ROL","acc"),("PLD","imp"),("BIT","abs"),("AND","abs"),("ROL","abs"),("AND","long")],
[("BMI","rel8"),("AND","dpiy"),("AND","dpi"),("AND","sriy"),("BIT","dpx"),("AND","dpx"),("ROL","dpx"),("AND","dply"),("SEC","imp"),("AND","absy"),("DEC","acc"),("TSC","imp"),("BIT","absx"),("AND","absx"),("ROL","absx"),("AND","longx")],
[("RTI","imp"),("EOR","dpix"),("WDM","imm8"),("EOR","sr"),("MVP","block"),("EOR","dp"),("LSR","dp"),("EOR","dpl"),("PHA","imp"),("EOR","immM"),("LSR","acc"),("PHK","imp"),("JMP","abs"),("EOR","abs"),("LSR","abs"),("EOR","long")],
[("BVC","rel8"),("EOR","dpiy"),("EOR","dpi"),("EOR","sriy"),("MVN","block"),("EOR","dpx"),("LSR","dpx"),("EOR","dply"),("CLI","imp"),("EOR","absy"),("PHY","imp"),("TCD","imp"),("JML","long"),("EOR","absx"),("LSR","absx"),("EOR","longx")],
[("RTS","imp"),("ADC","dpix"),("PER","rel16"),("ADC","sr"),("STZ","dp"),("ADC","dp"),("ROR","dp"),("ADC","dpl"),("PLA","imp"),("ADC","immM"),("ROR","acc"),("RTL","imp"),("JMP","absi"),("ADC","abs"),("ROR","abs"),("ADC","long")],
[("BVS","rel8"),("ADC","dpiy"),("ADC","dpi"),("ADC","sriy"),("STZ","dpx"),("ADC","dpx"),("ROR","dpx"),("ADC","dply"),("SEI","imp"),("ADC","absy"),("PLY","imp"),("TDC","imp"),("JMP","absix"),("ADC","absx"),("ROR","absx"),("ADC","longx")],
[("BRA","rel8"),("STA","dpix"),("BRL","rel16"),("STA","sr"),("STY","dp"),("STA","dp"),("STX","dp"),("STA","dpl"),("DEY","imp"),("BIT","immM"),("TXA","imp"),("PHB","imp"),("STY","abs"),("STA","abs"),("STX","abs"),("STA","long")],
[("BCC","rel8"),("STA","dpiy"),("STA","dpi"),("STA","sriy"),("STY","dpx"),("STA","dpx"),("STX","dpy"),("STA","dply"),("TYA","imp"),("STA","absy"),("TXS","imp"),("TXY","imp"),("STZ","abs"),("STA","absx"),("STZ","absx"),("STA","longx")],
[("LDY","immX"),("LDA","dpix"),("LDX","immX"),("LDA","sr"),("LDY","dp"),("LDA","dp"),("LDX","dp"),("LDA","dpl"),("TAY","imp"),("LDA","immM"),("TAX","imp"),("PLB","imp"),("LDY","abs"),("LDA","abs"),("LDX","abs"),("LDA","long")],
[("BCS","rel8"),("LDA","dpiy"),("LDA","dpi"),("LDA","sriy"),("LDY","dpx"),("LDA","dpx"),("LDX","dpy"),("LDA","dply"),("CLV","imp"),("LDA","absy"),("TSX","imp"),("TYX","imp"),("LDY","absx"),("LDA","absx"),("LDX","absy"),("LDA","longx")],
[("CPY","immX"),("CMP","dpix"),("REP","imm8"),("CMP","sr"),("CPY","dp"),("CMP","dp"),("DEC","dp"),("CMP","dpl"),("INY","imp"),("CMP","immM"),("DEX","imp"),("WAI","imp"),("CPY","abs"),("CMP","abs"),("DEC","abs"),("CMP","long")],
[("BNE","rel8"),("CMP","dpiy"),("CMP","dpi"),("CMP","sriy"),("PEI","dpi"),("CMP","dpx"),("DEC","dpx"),("CMP","dply"),("CLD","imp"),("CMP","absy"),("PHX","imp"),("STP","imp"),("JML","absl"),("CMP","absx"),("DEC","absx"),("CMP","longx")],
[("CPX","immX"),("SBC","dpix"),("SEP","imm8"),("SBC","sr"),("CPX","dp"),("SBC","dp"),("INC","dp"),("SBC","dpl"),("INX","imp"),("SBC","immM"),("NOP","imp"),("XBA","imp"),("CPX","abs"),("SBC","abs"),("INC","abs"),("SBC","long")],
[("BEQ","rel8"),("SBC","dpiy"),("SBC","dpi"),("SBC","sriy"),("PEA","abs"),("SBC","dpx"),("INC","dpx"),("SBC","dply"),("SED","imp"),("SBC","absy"),("PLX","imp"),("XCE","imp"),("JSR","absix"),("SBC","absx"),("INC","absx"),("SBC","longx")],
]
OPS = [op for row in ROWS for op in row]

@dataclass
class WidthState:
    m8: bool = True
    x8: bool = True


def parse_pc(value: str) -> int:
    bank, addr = value.split(":", 1)
    return (int(bank, 16) << 16) | int(addr, 16)


def hirom_offset(pc: int) -> int:
    bank = (pc >> 16) & 0xFF
    addr = pc & 0xFFFF
    if addr < 0x8000 and bank < 0x40:
        raise ValueError(f"${pc:06X} is not ROM in the low HiROM banks")
    return ((bank & 0x3F) << 16) | addr


def operand_size(mode: str, state: WidthState) -> int:
    size = MODES[mode]
    if size == -1:
        return 1 if state.m8 else 2
    if size == -2:
        return 1 if state.x8 else 2
    return size


def fmt_operand(mode: str, raw: bytes, next_pc: int) -> str:
    if not raw:
        return "A" if mode == "acc" else ""
    value = int.from_bytes(raw, "little")
    if mode == "rel8":
        disp = value if value < 0x80 else value - 0x100
        return f"${(next_pc + disp) & 0xFFFFFF:06X}"
    if mode == "rel16":
        disp = value if value < 0x8000 else value - 0x10000
        return f"${(next_pc + disp) & 0xFFFFFF:06X}"
    if mode == "block":
        return f"${raw[0]:02X},${raw[1]:02X}"
    width = len(raw) * 2
    prefix = "#$" if mode.startswith("imm") else "$"
    suffixes = {"dpx":",X","dpy":",Y","absx":",X","absy":",Y","longx":",X","dpiy":") ,Y","dply":"] ,Y","sr":",S","sriy":",S),Y"}
    wrappers = {"dpi":("(",")"),"dpix":("(",",X)"),"dpl":("[","]"),"dpiy":("(",") ,Y"),"dply":("[","] ,Y"),"absi":("(",")"),"absix":("(",",X)"),"absl":("[","]"),"sriy":("(",",S),Y")}
    if mode in wrappers:
        left, right = wrappers[mode]
        return f"{left}${value:0{width}X}{right}"
    return f"{prefix}{value:0{width}X}{suffixes.get(mode, '')}"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--pc", default="00:8000")
    parser.add_argument("--count", type=int, default=32)
    parser.add_argument("--m16", action="store_true")
    parser.add_argument("--x16", action="store_true")
    args = parser.parse_args()

    rom = args.rom.read_bytes()
    pc = parse_pc(args.pc)
    state = WidthState(m8=not args.m16, x8=not args.x16)

    for _ in range(args.count):
        off = hirom_offset(pc)
        opcode = rom[off]
        mnemonic, mode = OPS[opcode]
        size = operand_size(mode, state)
        raw = rom[off + 1 : off + 1 + size]
        if len(raw) != size:
            raise ValueError("instruction crosses end of ROM")
        next_pc = (pc + 1 + size) & 0xFFFFFF
        operand = fmt_operand(mode, raw, next_pc)
        bytes_text = " ".join(f"{b:02X}" for b in bytes([opcode]) + raw)
        print(f"{pc:06X}  {bytes_text:<11} {mnemonic} {operand}".rstrip())

        if mnemonic == "REP" and raw:
            state.m8 = state.m8 and not bool(raw[0] & 0x20)
            state.x8 = state.x8 and not bool(raw[0] & 0x10)
        elif mnemonic == "SEP" and raw:
            state.m8 = state.m8 or bool(raw[0] & 0x20)
            state.x8 = state.x8 or bool(raw[0] & 0x10)
        pc = next_pc
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
