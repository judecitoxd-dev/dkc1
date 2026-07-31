#ifndef DK1_SPC700_BOOTSTRAP_H
#define DK1_SPC700_BOOTSTRAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/apu_boot_image.h"

#define DK1_SPC700_MEMORY_SIZE 65536u
#define DK1_SPC700_BOOT_PC 0x04B8u
#define DK1_SPC700_TOKEN_ADDRESS 0x04DEu
#define DK1_SPC700_PORT0 0x00F4u
#define DK1_SPC700_PORT1 0x00F5u
#define DK1_SPC700_PORT2 0x00F6u
#define DK1_SPC700_PORT3 0x00F7u

typedef enum Dk1Spc700StepStatus {
    DK1_SPC700_STEP_OK = 0,
    DK1_SPC700_STEP_JUMPED_TO_PAYLOAD,
    DK1_SPC700_STEP_UNSUPPORTED_OPCODE,
    DK1_SPC700_STEP_LIMIT
} Dk1Spc700StepStatus;

typedef struct Dk1Spc700BootstrapCpu {
    uint8_t memory[DK1_SPC700_MEMORY_SIZE];
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    bool zero;
    bool carry;
    uint64_t instructions;
    uint8_t unsupported_opcode;
    bool payload_started;
} Dk1Spc700BootstrapCpu;

void dk1_spc700_bootstrap_init(Dk1Spc700BootstrapCpu *cpu,
                               const Dk1ApuBootImage *image);
Dk1Spc700StepStatus dk1_spc700_bootstrap_step(Dk1Spc700BootstrapCpu *cpu);

/* Host-side protocol helpers that execute the uploaded 40-byte program itself. */
bool dk1_spc700_bootstrap_send_byte(Dk1Spc700BootstrapCpu *cpu,
                                    uint8_t token, uint16_t destination,
                                    uint8_t value, size_t max_steps);
bool dk1_spc700_bootstrap_launch(Dk1Spc700BootstrapCpu *cpu,
                                 uint8_t token, uint16_t entry,
                                 size_t max_steps);

#endif
