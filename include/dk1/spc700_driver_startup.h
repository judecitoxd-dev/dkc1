#ifndef DK1_SPC700_DRIVER_STARTUP_H
#define DK1_SPC700_DRIVER_STARTUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_SPC_RAM_SIZE 65536u
#define DK1_SPC_DRIVER_CODE_SOURCE 0x8AA36Eu
#define DK1_SPC_DRIVER_CODE_TARGET 0x04E8u
#define DK1_SPC_DRIVER_CODE_SIZE 0x0D7Bu
#define DK1_SPC_DRIVER_DATA_BLOCK 0xC92D95u
#define DK1_SPC_DRIVER_ENTRY 0x05E8u

typedef struct Dk1Spc700DriverStartup {
    uint8_t ram[DK1_SPC_RAM_SIZE];
    uint16_t data_target;
    uint16_t data_length;
    uint64_t code_signature;
    uint64_t data_signature;
    uint64_t ram_signature;
} Dk1Spc700DriverStartup;

typedef enum Dk1Spc700TraceStop {
    DK1_SPC_TRACE_LIMIT = 0,
    DK1_SPC_TRACE_UNSUPPORTED_OPCODE = 1
} Dk1Spc700TraceStop;

typedef struct Dk1Spc700DriverTrace {
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t sp;
    uint8_t psw;
    uint64_t instructions;
    uint16_t unsupported_pc;
    uint8_t unsupported_opcode;
    Dk1Spc700TraceStop stop;
} Dk1Spc700DriverTrace;

bool dk1_spc700_driver_startup_load(const Dk1RomImage *rom,
                                    Dk1Spc700DriverStartup *startup);
bool dk1_spc700_driver_trace_entry(Dk1Spc700DriverStartup *startup,
                                   uint8_t initial_port0,
                                   size_t instruction_limit,
                                   Dk1Spc700DriverTrace *trace);

#endif
