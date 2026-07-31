#include <assert.h>
#include <string.h>
#include "dk1/spc700_bootstrap.h"

int main(void) {
    static const uint8_t code[DK1_APU_BOOT_IMAGE_SIZE] = {
        0xCD,0x00,0xE4,0xF4,0x64,0xF4,0xD0,0xFA,0x65,0xDE,0x04,0xF0,0xF5,
        0xC5,0xDE,0x04,0x28,0x01,0xF0,0x0F,0xE4,0xF5,0x64,0xF5,0xD0,0xFA,
        0xC7,0xF6,0xE5,0xDE,0x04,0xC4,0xF4,0x2F,0xDF,0x1F,0xF6,0x00,0x00,0x00
    };
    Dk1ApuBootImage image;
    Dk1Spc700BootstrapCpu cpu;
    memset(&image, 0, sizeof(image));
    memcpy(image.bytes, code, sizeof(code));
    dk1_spc700_bootstrap_init(&cpu, &image);
    assert(cpu.pc == DK1_SPC700_BOOT_PC);

    /* First odd token uploads one byte through the code's C7 $F6 indirect store. */
    assert(dk1_spc700_bootstrap_send_byte(&cpu, 1u, 0x2000u, 0x5Au, 128u));
    assert(cpu.memory[0x2000u] == 0x5Au);
    assert(cpu.memory[DK1_SPC700_TOKEN_ADDRESS] == 1u);

    assert(dk1_spc700_bootstrap_send_byte(&cpu, 3u, 0x2001u, 0xA5u, 128u));
    assert(cpu.memory[0x2001u] == 0xA5u);

    /* An even token follows the bootstrap's 1F F6 00 indirect jump. */
    assert(dk1_spc700_bootstrap_launch(&cpu, 4u, 0x2000u, 128u));
    assert(cpu.payload_started && cpu.pc == 0x2000u);
    assert(cpu.unsupported_opcode == 0u);
    return 0;
}
