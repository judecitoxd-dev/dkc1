#include "dk1/object_frame_layout.h"
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

static uint16_t next_large_tile(uint16_t tile) {
    tile = (uint16_t)(tile + 2u);
    if ((tile & 0x0010u) != 0u) tile = (uint16_t)(tile + 0x0010u);
    return tile;
}

static uint64_t hash_byte(uint64_t hash, uint8_t byte) {
    hash ^= byte;
    return hash * 1099511628211ull;
}

bool dk1_object_frame_layout_read(const Dk1RomImage *rom, uint16_t frame_id,
                                  Dk1ObjectFrameLayout *layout) {
    uint16_t address, bank_word;
    uint8_t header[8];
    size_t i, cursor = 0u;
    uint16_t tile;
    if (rom == NULL || layout == NULL || (frame_id & 1u) != 0u ||
        !dk1_rom_read_u16(rom, DK1_OBJECT_FRAME_POINTER_TABLE + frame_id, &address) ||
        !dk1_rom_read_u16(rom, DK1_OBJECT_FRAME_POINTER_TABLE + frame_id + 2u, &bank_word))
        return false;
    memset(layout, 0, sizeof(*layout));
    layout->frame_id = frame_id;
    layout->source_pc = ((uint32_t)(bank_word & 0x00FFu) << 16u) | address;
    if (!dk1_rom_copy(rom, layout->source_pc, header, sizeof(header))) return false;
    layout->large_count = header[0];
    layout->small_count = header[1];
    layout->small_tile_offset = header[2];
    layout->auxiliary_count = header[3];
    layout->auxiliary_tile_offset = header[4];
    layout->attribute_5 = header[5];
    layout->attribute_6 = header[6];
    layout->attribute_7 = header[7];
    layout->piece_count = (size_t)layout->large_count + layout->small_count + layout->auxiliary_count;
    if (layout->piece_count > DK1_OBJECT_FRAME_MAX_PIECES) return false;

    tile = 0u;
    for (i = 0u; i < layout->large_count; ++i, ++cursor) {
        uint8_t pair[2];
        if (!dk1_rom_copy(rom, layout->source_pc + 8u + cursor * 2u, pair, 2u)) return false;
        layout->pieces[cursor] = (Dk1ObjectFramePiece){
            pair[0], pair[1], (int16_t)((int)pair[0] - 0x80),
            (int16_t)((int)pair[1] - 0x70), tile, DK1_OBJECT_FRAME_LARGE};
        tile = next_large_tile(tile);
    }
    tile = layout->small_tile_offset;
    for (i = 0u; i < layout->small_count; ++i, ++cursor) {
        uint8_t pair[2];
        if (!dk1_rom_copy(rom, layout->source_pc + 8u + cursor * 2u, pair, 2u)) return false;
        layout->pieces[cursor] = (Dk1ObjectFramePiece){
            pair[0], pair[1], (int16_t)((int)pair[0] - 0x80),
            (int16_t)((int)pair[1] - 0x70), tile++, DK1_OBJECT_FRAME_SMALL};
    }
    tile = layout->auxiliary_tile_offset;
    for (i = 0u; i < layout->auxiliary_count; ++i, ++cursor) {
        uint8_t pair[2];
        if (!dk1_rom_copy(rom, layout->source_pc + 8u + cursor * 2u, pair, 2u)) return false;
        layout->pieces[cursor] = (Dk1ObjectFramePiece){
            pair[0], pair[1], (int16_t)((int)pair[0] - 0x80),
            (int16_t)((int)pair[1] - 0x70), tile++, DK1_OBJECT_FRAME_AUXILIARY};
    }
    return true;
}

bool dk1_object_frame_layout_emit_oam(const Dk1ObjectFrameLayout *layout,
                                      int16_t base_x, int16_t base_y,
                                      Dk1ObjectFrameOamStyle style,
                                      uint8_t obsel, size_t first_sprite,
                                      Dk1OamImage *oam, size_t *written) {
    size_t i;
    if (layout == NULL || oam == NULL || first_sprite > DK1_OAM_SPRITE_COUNT ||
        layout->piece_count > DK1_OAM_SPRITE_COUNT - first_sprite) return false;
    for (i = 0u; i < layout->piece_count; ++i) {
        const Dk1ObjectFramePiece *piece = &layout->pieces[i];
        Dk1OamSprite sprite;
        int16_t x = (int16_t)(base_x + piece->encoded_x);
        int16_t y = (int16_t)(base_y + piece->encoded_y);
        const bool large = piece->group == DK1_OBJECT_FRAME_LARGE;
        sprite.x = x;
        sprite.y = (uint8_t)y;
        sprite.tile_number = (uint16_t)((style.base_tile_number + piece->tile_offset) & 0x01FFu);
        sprite.palette = style.palette;
        sprite.priority = style.priority;
        sprite.horizontal_flip = style.horizontal_flip;
        sprite.vertical_flip = style.vertical_flip;
        sprite.large = large;
        size_pair((uint8_t)(obsel >> 5u), large, &sprite.width, &sprite.height);
        if (!dk1_oam_encode_sprite(oam, obsel, first_sprite + i, &sprite)) return false;
    }
    if (written != NULL) *written = layout->piece_count;
    return true;
}

uint64_t dk1_object_frame_layout_signature(const Dk1ObjectFrameLayout *layout) {
    uint64_t hash = 14695981039346656037ull;
    size_t i;
    if (layout == NULL) return 0u;
    for (i = 0u; i < 4u; ++i) hash = hash_byte(hash, (uint8_t)(layout->source_pc >> (i * 8u)));
    hash = hash_byte(hash, layout->large_count);
    hash = hash_byte(hash, layout->small_count);
    hash = hash_byte(hash, layout->small_tile_offset);
    hash = hash_byte(hash, layout->auxiliary_count);
    hash = hash_byte(hash, layout->auxiliary_tile_offset);
    for (i = 0u; i < layout->piece_count; ++i) {
        hash = hash_byte(hash, layout->pieces[i].encoded_x);
        hash = hash_byte(hash, layout->pieces[i].encoded_y);
        hash = hash_byte(hash, (uint8_t)layout->pieces[i].tile_offset);
        hash = hash_byte(hash, (uint8_t)layout->pieces[i].group);
    }
    return hash;
}
