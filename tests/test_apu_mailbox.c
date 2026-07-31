#include <assert.h>
#include "dk1/apu_mailbox.h"
int main(void) {
    Dk1ApuMailbox m; dk1_apu_mailbox_init(&m);
    assert(dk1_apu_mailbox_ready(&m));
    assert(dk1_apu_mailbox_send_command(&m,0x2Au));
    assert(m.cpu_to_apu.port[1]==0x2Au && m.cpu_to_apu.port[0]==0x81u);
    assert(!dk1_apu_mailbox_send_command(&m,0x2Bu));
    dk1_apu_mailbox_acknowledge(&m);
    assert(dk1_apu_mailbox_send_command(&m,0x2Bu));
    dk1_apu_mailbox_init(&m);
    assert(!dk1_apu_boot_begin(&m,0u));
    assert(dk1_apu_boot_begin(&m,DK1_APU_BOOT_SIGNATURE));
    assert(m.cpu_to_apu.port[2]==0xB8u && m.cpu_to_apu.port[3]==0x04u);
    assert(m.cpu_to_apu.port[0]==0xCCu && m.cpu_to_apu.port[1]==0x01u);
    m.apu_to_cpu.port[0]=3u;
    assert(dk1_apu_boot_upload_byte(&m,3u,0x55u));
    assert(m.cpu_to_apu.port[1]==0x55u && m.cpu_to_apu.port[0]==3u);
    assert(!dk1_apu_boot_upload_byte(&m,0x28u,0u));
    return 0;
}
