#include "dk1/object_screen_oam.h"
#include <string.h>

static void size_pair(uint8_t mode, bool large, uint8_t *width, uint8_t *height) {
    static const uint8_t sizes[8][4] = {
        {8,8,16,16}, {8,8,32,32}, {8,8,64,64}, {16,16,32,32},
        {16,16,64,64}, {32,32,64,64}, {16,32,32,64}, {16,32,32,32}
    };
    const uint8_t *entry = sizes[mode & 7u];
    *width = entry[large ? 2u : 0u];
    *height = entry[large ? 3u : 1u];
}

static int16_t wrap_oam_x(int32_t x) {
    int32_t wrapped = x % 512;
    if (wrapped < 0) wrapped += 512;
    if (wrapped >= 256) wrapped -= 512;
    return (int16_t)wrapped;
}

bool dk1_object_frame_emit_screen_oam(const Dk1ObjectFrameLayout *layout,
                                      Dk1ObjectScreenTransform t,
                                      Dk1ObjectFrameOamStyle style,
                                      uint8_t obsel,
                                      size_t first_sprite,
                                      Dk1OamImage *oam,
                                      Dk1ObjectScreenOamStats *stats) {
    const bool hflip = (t.object_flags & 0x4000u) != 0u;
    const bool vflip = (t.object_flags & 0x8000u) != 0u;
    const int32_t center_x = (int32_t)t.world_x - t.camera_x;
    const int32_t center_y = (int32_t)t.vertical_origin - t.world_y -
                             t.vertical_scroll + t.vertical_offset;
    size_t i, out = 0u;
    Dk1ObjectScreenOamStats local = {0};
    if (layout == NULL || oam == NULL || first_sprite > DK1_OAM_SPRITE_COUNT) return false;
    for (i = 0u; i < layout->piece_count; ++i) {
        const Dk1ObjectFramePiece *piece = &layout->pieces[i];
        const bool large = piece->group == DK1_OBJECT_FRAME_LARGE;
        Dk1OamSprite sprite;
        int32_t x, y;
        uint8_t width, height;
        local.considered++;
        size_pair((uint8_t)(obsel >> 5u), large, &width, &height);
        x = hflip ? center_x - piece->x_offset - width : center_x + piece->x_offset;
        y = vflip ? center_y - piece->y_offset - height : center_y + piece->y_offset;
        if ((uint16_t)y >= 0x00F0u) {
            local.vertically_clipped++;
            continue;
        }
        if (first_sprite + out >= DK1_OAM_SPRITE_COUNT) return false;
        sprite.x = wrap_oam_x(x);
        sprite.y = (uint8_t)y;
        sprite.tile_number = (uint16_t)((style.base_tile_number + piece->tile_offset) & 0x01FFu);
        sprite.palette = style.palette;
        sprite.priority = style.priority;
        sprite.horizontal_flip = hflip;
        sprite.vertical_flip = vflip;
        sprite.large = large;
        sprite.width = width;
        sprite.height = height;
        if (!dk1_oam_encode_sprite(oam, obsel, first_sprite + out, &sprite)) return false;
        out++;
    }
    local.written = out;
    if (stats != NULL) *stats = local;
    return true;
}
