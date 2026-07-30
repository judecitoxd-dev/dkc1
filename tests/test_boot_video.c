#include <assert.h>
#include <stddef.h>
#include "dk1/boot_video.h"

int main(void) {
    const Dk1BootVideoConfig config = dk1_boot_video_config();
    assert(config.nmitimen == 0x01);
    assert(config.inidisp == 0x8F);
    assert(config.bgmode == 0x03);
    assert(config.main_screen == 0x02);
    assert(config.sub_screen == 0x01);
    assert(config.bg1sc == 0x7C);
    assert(config.bg2sc == 0x78);
    assert(config.vram_words_cleared == 0x4000u);
    assert(config.high_byte_upload_bytes == 0x3400u);
    assert(config.high_byte_source_snes == 0xEE964Bu);

    size_t count = 0;
    const Dk1VramUploadRequest *uploads = dk1_boot_vram_uploads(&count);
    assert(uploads != NULL);
    assert(count == 7u);
    assert(uploads[0].target_word == 0x7054u);
    assert(uploads[0].source_snes == 0xED2398u);
    assert(uploads[6].target_word == 0x6000u);
    assert(uploads[6].requested_bytes == 0x1000u);
    return 0;
}
