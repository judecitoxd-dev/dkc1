# Boot video configuration

The block beginning at `$00:BA91` establishes an initial forced-blank PPU state before uploading boot graphics.

Confirmed register values include NMITIMEN `$01`, INIDISP `$8F`, BGMODE `$03`, TM `$02`, TS `$01`, CGWSEL `$02`, CGADSUB `$02`, BG12NBA `$64`, BG34NBA `$03`, BG1SC `$7C`, BG2SC `$78`, and BG3SC `$70`.

The code then clears `0x4000` VRAM words and performs an observed `0x3400`-byte high-byte upload from `$EE:964B`. Seven subsequent transfer requests to `$B9:99AD` are represented in portable C as metadata only; no source asset bytes are committed.
