#include "dk1/boot_video.h"

Dk1BootVideoConfig dk1_boot_video_config(void) {
    const Dk1BootVideoConfig config = {
        .nmitimen = 0x01,
        .inidisp = 0x8F,
        .obsel = 0x00,
        .setini = 0x00,
        .bgmode = 0x03,
        .mosaic = 0x00,
        .main_screen = 0x02,
        .sub_screen = 0x01,
        .cgwsel = 0x02,
        .cgadsub = 0x02,
        .bg12nba = 0x64,
        .bg34nba = 0x03,
        .bg1sc = 0x7C,
        .bg2sc = 0x78,
        .bg3sc = 0x70,
        .bg4sc = 0x00,
        .vram_words_cleared = 0x4000u,
        .high_byte_upload_bytes = 0x3400u,
        .high_byte_source_snes = 0xEE964Bu,
        .high_byte_target_word = 0x0000u,
    };
    return config;
}

const Dk1VramUploadRequest *dk1_boot_vram_uploads(size_t *count) {
    static const Dk1VramUploadRequest requests[] = {
        {0x7054u, 0xED2398u, 0x05ACu},
        {0x74A0u, 0xE6BEE4u, 0x0340u},
        {0x76BAu, 0xD232E0u, 0x00C8u},
        {0x78E0u, 0xED2944u, 0x0380u},
        {0x7CE0u, 0xE193CBu, 0x0380u},
        {0x4000u, 0xE1974Bu, 0x2400u},
        {0x6000u, 0xED2CC4u, 0x1000u},
    };
    if (count != NULL) {
        *count = sizeof(requests) / sizeof(requests[0]);
    }
    return requests;
}
