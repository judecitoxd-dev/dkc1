#ifndef DK1_APU_MAILBOX_H
#define DK1_APU_MAILBOX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_APU_BOOT_SIGNATURE 0xBBAAu
#define DK1_APU_BOOT_ADDRESS 0x04B8u
#define DK1_APU_BOOT_COMMAND 0x01CCu

typedef struct Dk1ApuPorts { uint8_t port[4]; } Dk1ApuPorts;
typedef struct Dk1ApuMailbox {
    Dk1ApuPorts cpu_to_apu;
    Dk1ApuPorts apu_to_cpu;
    uint8_t token;
    uint32_t commands_sent;
    uint32_t waits;
} Dk1ApuMailbox;

void dk1_apu_mailbox_init(Dk1ApuMailbox *mailbox);
bool dk1_apu_mailbox_ready(const Dk1ApuMailbox *mailbox);
bool dk1_apu_mailbox_send_command(Dk1ApuMailbox *mailbox, uint8_t command);
void dk1_apu_mailbox_acknowledge(Dk1ApuMailbox *mailbox);
bool dk1_apu_boot_begin(Dk1ApuMailbox *mailbox, uint16_t apu_signature);
bool dk1_apu_boot_upload_byte(Dk1ApuMailbox *mailbox, uint8_t index, uint8_t value);

#endif
