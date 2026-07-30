#ifndef DK1_SNES_REGISTERS_H
#define DK1_SNES_REGISTERS_H

#include <stdint.h>

enum Dk1SnesRegister {
    DK1_REG_INIDISP = 0x2100,
    DK1_REG_OAMADDL = 0x2102,
    DK1_REG_VMADDL = 0x2116,
    DK1_REG_VMDATAL = 0x2118,
    DK1_REG_WMDATA = 0x2180,
    DK1_REG_WMADDL = 0x2181,
    DK1_REG_WMADDM = 0x2182,
    DK1_REG_WMADDH = 0x2183,
    DK1_REG_NMITIMEN = 0x4200,
    DK1_REG_MDMAEN = 0x420B,
    DK1_REG_HDMAEN = 0x420C,
    DK1_REG_MEMSEL = 0x420D,
    DK1_REG_DMAP0 = 0x4300,
    DK1_REG_BBAD0 = 0x4301,
    DK1_REG_A1T0L = 0x4302,
    DK1_REG_A1B0 = 0x4304,
    DK1_REG_DAS0L = 0x4305,
};

static inline uint32_t dk1_snes_dma_length(uint16_t das) {
    return das == 0 ? 65536u : (uint32_t)das;
}

#endif
