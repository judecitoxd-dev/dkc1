#ifndef DK1_RESET_STATE_H
#define DK1_RESET_STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Dk1BootState {
    bool native_mode;
    bool interrupts_disabled;
    bool decimal_mode;
    bool fast_rom_enabled;
    uint8_t data_bank;
    uint16_t direct_page;
    uint16_t stack_pointer;
} Dk1BootState;

/*
 * Clean-room behavioral model of the state established by the reset entry.
 * Source reference: SNES CPU address $00:8000 in the verified Rev 2 ROM.
 */
Dk1BootState dk1_boot_state_after_entry(void);

#endif
