#ifndef DK1_BOOT_VIDEO_H
#define DK1_BOOT_VIDEO_H

#include <stddef.h>
#include <stdint.h>

typedef struct Dk1BootVideoConfig {
    uint8_t nmitimen;
    uint8_t inidisp;
    uint8_t obsel;
    uint8_t setini;
    uint8_t bgmode;
    uint8_t mosaic;
    uint8_t main_screen;
    uint8_t sub_screen;
    uint8_t cgwsel;
    uint8_t cgadsub;
    uint8_t bg12nba;
    uint8_t bg34nba;
    uint8_t bg1sc;
    uint8_t bg2sc;
    uint8_t bg3sc;
    uint8_t bg4sc;
    uint32_t vram_words_cleared;
    uint32_t high_byte_upload_bytes;
    uint32_t high_byte_source_snes;
    uint16_t high_byte_target_word;
} Dk1BootVideoConfig;

typedef struct Dk1VramUploadRequest {
    uint16_t target_word;
    uint32_t source_snes;
    uint16_t requested_bytes;
} Dk1VramUploadRequest;

Dk1BootVideoConfig dk1_boot_video_config(void);
const Dk1VramUploadRequest *dk1_boot_vram_uploads(size_t *count);

#endif
