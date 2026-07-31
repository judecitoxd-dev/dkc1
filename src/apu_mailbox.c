#include "dk1/apu_mailbox.h"
#include <string.h>

void dk1_apu_mailbox_init(Dk1ApuMailbox *mailbox) {
    if (mailbox != NULL) memset(mailbox, 0, sizeof(*mailbox));
}

bool dk1_apu_mailbox_ready(const Dk1ApuMailbox *mailbox) {
    return mailbox != NULL && mailbox->apu_to_cpu.port[0] == mailbox->token;
}

/* Translation of the command handshake at $CA:B1AF-$CA:B1CA. */
bool dk1_apu_mailbox_send_command(Dk1ApuMailbox *mailbox, uint8_t command) {
    if (mailbox == NULL) return false;
    if (!dk1_apu_mailbox_ready(mailbox)) {
        ++mailbox->waits;
        return false;
    }
    mailbox->cpu_to_apu.port[1] = command;
    mailbox->token = (uint8_t)((mailbox->token ^ 0x80u) | 0x01u);
    mailbox->cpu_to_apu.port[0] = mailbox->token;
    ++mailbox->commands_sent;
    return true;
}

void dk1_apu_mailbox_acknowledge(Dk1ApuMailbox *mailbox) {
    if (mailbox != NULL) mailbox->apu_to_cpu.port[0] = mailbox->cpu_to_apu.port[0];
}

/* First half of the IPL handshake at $CA:B0EE-$CA:B10C. */
bool dk1_apu_boot_begin(Dk1ApuMailbox *mailbox, uint16_t apu_signature) {
    if (mailbox == NULL || apu_signature != DK1_APU_BOOT_SIGNATURE) return false;
    mailbox->cpu_to_apu.port[2] = (uint8_t)DK1_APU_BOOT_ADDRESS;
    mailbox->cpu_to_apu.port[3] = (uint8_t)(DK1_APU_BOOT_ADDRESS >> 8u);
    mailbox->cpu_to_apu.port[0] = (uint8_t)DK1_APU_BOOT_COMMAND;
    mailbox->cpu_to_apu.port[1] = (uint8_t)(DK1_APU_BOOT_COMMAND >> 8u);
    mailbox->token = mailbox->cpu_to_apu.port[0];
    return true;
}

/* Models the indexed 40-byte boot upload loop at $CA:B10C-$CA:B12C. */
bool dk1_apu_boot_upload_byte(Dk1ApuMailbox *mailbox, uint8_t index, uint8_t value) {
    if (mailbox == NULL || index >= 0x28u || mailbox->apu_to_cpu.port[0] != index) return false;
    mailbox->cpu_to_apu.port[1] = value;
    mailbox->cpu_to_apu.port[0] = index;
    return true;
}
