#include "dk1/spc700_bootstrap.h"
#include <string.h>

static uint16_t read16(const Dk1Spc700BootstrapCpu *cpu, uint16_t address) {
    return (uint16_t)(cpu->memory[address] |
        ((uint16_t)cpu->memory[(uint16_t)(address + 1u)] << 8u));
}

static void set_nz(Dk1Spc700BootstrapCpu *cpu, uint8_t value) {
    cpu->zero = value == 0u;
}

static void compare_a(Dk1Spc700BootstrapCpu *cpu, uint8_t value) {
    cpu->zero = cpu->a == value;
    cpu->carry = cpu->a >= value;
}

static uint16_t relative_target(uint16_t pc_after_operand, uint8_t raw) {
    const int8_t displacement = (int8_t)raw;
    return (uint16_t)(pc_after_operand + displacement);
}

void dk1_spc700_bootstrap_init(Dk1Spc700BootstrapCpu *cpu,
                               const Dk1ApuBootImage *image) {
    if (cpu == NULL) return;
    memset(cpu, 0, sizeof(*cpu));
    cpu->pc = DK1_SPC700_BOOT_PC;
    if (image != NULL)
        memcpy(cpu->memory + DK1_SPC700_BOOT_PC, image->bytes, DK1_APU_BOOT_IMAGE_SIZE);
}

Dk1Spc700StepStatus dk1_spc700_bootstrap_step(Dk1Spc700BootstrapCpu *cpu) {
    uint8_t opcode;
    uint8_t operand;
    uint16_t address;
    uint16_t target;
    if (cpu == NULL) return DK1_SPC700_STEP_UNSUPPORTED_OPCODE;
    opcode = cpu->memory[cpu->pc++];
    ++cpu->instructions;
    switch (opcode) {
    case 0xCDu: /* MOV X,#imm */
        cpu->x = cpu->memory[cpu->pc++];
        set_nz(cpu, cpu->x);
        return DK1_SPC700_STEP_OK;
    case 0xE4u: /* MOV A,dp */
        operand = cpu->memory[cpu->pc++];
        cpu->a = cpu->memory[operand];
        set_nz(cpu, cpu->a);
        return DK1_SPC700_STEP_OK;
    case 0x64u: /* CMP A,dp */
        operand = cpu->memory[cpu->pc++];
        compare_a(cpu, cpu->memory[operand]);
        return DK1_SPC700_STEP_OK;
    case 0x65u: /* CMP A,!abs */
        address = read16(cpu, cpu->pc);
        cpu->pc = (uint16_t)(cpu->pc + 2u);
        compare_a(cpu, cpu->memory[address]);
        return DK1_SPC700_STEP_OK;
    case 0xD0u: /* BNE rel */
        operand = cpu->memory[cpu->pc++];
        if (!cpu->zero) cpu->pc = relative_target(cpu->pc, operand);
        return DK1_SPC700_STEP_OK;
    case 0xF0u: /* BEQ rel */
        operand = cpu->memory[cpu->pc++];
        if (cpu->zero) cpu->pc = relative_target(cpu->pc, operand);
        return DK1_SPC700_STEP_OK;
    case 0xC5u: /* MOV !abs,A */
        address = read16(cpu, cpu->pc);
        cpu->pc = (uint16_t)(cpu->pc + 2u);
        cpu->memory[address] = cpu->a;
        return DK1_SPC700_STEP_OK;
    case 0x28u: /* AND A,#imm */
        cpu->a &= cpu->memory[cpu->pc++];
        set_nz(cpu, cpu->a);
        return DK1_SPC700_STEP_OK;
    case 0xC7u: /* MOV [dp+X],A */
        operand = cpu->memory[cpu->pc++];
        operand = (uint8_t)(operand + cpu->x);
        target = (uint16_t)(cpu->memory[operand] |
            ((uint16_t)cpu->memory[(uint8_t)(operand + 1u)] << 8u));
        cpu->memory[target] = cpu->a;
        return DK1_SPC700_STEP_OK;
    case 0xE5u: /* MOV A,!abs */
        address = read16(cpu, cpu->pc);
        cpu->pc = (uint16_t)(cpu->pc + 2u);
        cpu->a = cpu->memory[address];
        set_nz(cpu, cpu->a);
        return DK1_SPC700_STEP_OK;
    case 0xC4u: /* MOV dp,A */
        operand = cpu->memory[cpu->pc++];
        cpu->memory[operand] = cpu->a;
        return DK1_SPC700_STEP_OK;
    case 0x2Fu: /* BRA rel */
        operand = cpu->memory[cpu->pc++];
        cpu->pc = relative_target(cpu->pc, operand);
        return DK1_SPC700_STEP_OK;
    case 0x1Fu: /* JMP [!abs+X] */
        address = read16(cpu, cpu->pc);
        cpu->pc = (uint16_t)(cpu->pc + 2u);
        target = read16(cpu, (uint16_t)(address + cpu->x));
        cpu->pc = target;
        cpu->payload_started = true;
        return DK1_SPC700_STEP_JUMPED_TO_PAYLOAD;
    default:
        cpu->unsupported_opcode = opcode;
        return DK1_SPC700_STEP_UNSUPPORTED_OPCODE;
    }
}

bool dk1_spc700_bootstrap_send_byte(Dk1Spc700BootstrapCpu *cpu,
                                    uint8_t token, uint16_t destination,
                                    uint8_t value, size_t max_steps) {
    size_t i;
    Dk1Spc700StepStatus status;
    if (cpu == NULL || (token & 1u) == 0u || max_steps == 0u) return false;
    cpu->memory[DK1_SPC700_PORT0] = token;
    cpu->memory[DK1_SPC700_PORT1] = value;
    cpu->memory[DK1_SPC700_PORT2] = (uint8_t)destination;
    cpu->memory[DK1_SPC700_PORT3] = (uint8_t)(destination >> 8u);
    for (i = 0u; i < max_steps; ++i) {
        status = dk1_spc700_bootstrap_step(cpu);
        if (status != DK1_SPC700_STEP_OK) return false;
        if (cpu->memory[DK1_SPC700_TOKEN_ADDRESS] == token &&
            cpu->memory[DK1_SPC700_PORT0] == token &&
            cpu->memory[destination] == value && cpu->pc == 0x04BAu) return true;
    }
    return false;
}

bool dk1_spc700_bootstrap_launch(Dk1Spc700BootstrapCpu *cpu,
                                 uint8_t token, uint16_t entry,
                                 size_t max_steps) {
    size_t i;
    Dk1Spc700StepStatus status;
    if (cpu == NULL || (token & 1u) != 0u || max_steps == 0u) return false;
    cpu->memory[DK1_SPC700_PORT0] = token;
    cpu->memory[DK1_SPC700_PORT2] = (uint8_t)entry;
    cpu->memory[DK1_SPC700_PORT3] = (uint8_t)(entry >> 8u);
    for (i = 0u; i < max_steps; ++i) {
        status = dk1_spc700_bootstrap_step(cpu);
        if (status == DK1_SPC700_STEP_JUMPED_TO_PAYLOAD)
            return cpu->pc == entry;
        if (status != DK1_SPC700_STEP_OK) return false;
    }
    return false;
}
