#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/dynamic_bg1_runtime.h"
#include "dk1/dynamic_stream.h"

static void preset_register(Dk1PpuPreset *preset,
                            uint16_t address,
                            uint8_t value) {
    const size_t index = (size_t)(address - DK1_PPU_REGISTER_FIRST);
    assert(index < DK1_PPU_REGISTER_COUNT);
    preset->values[index] = value;
    preset->written[index] = true;
    ++preset->write_count;
}

int main(void) {
    static uint8_t data[DK1_EXPECTED_ROM_SIZE];
    Dk1RomImage rom;
    Dk1SceneMemory scene;
    Dk1DynamicBg1Runtime runtime;
    Dk1DynamicStreamState stream;
    Dk1DynamicStreamUpdate update;
    Dk1SceneView view;
    Dk1Rgba8 pixels[8u * 8u];
    Dk1RgbaSurface surface;
    uint64_t first_signature;
    size_t offset;
    size_t i;

    memset(&scene, 0, sizeof(scene));
    memset(&stream, 0, sizeof(stream));
    memset(data, 0, sizeof(data));
    assert(dk1_rom_image_init(&rom, data, sizeof(data)));

    scene.terrain.map_snes = 0xC00000u;
    scene.terrain.visual_bank = 0xC0u;
    scene.terrain.visual_block_base = 0x0100u;
    assert(dk1_hirom_offset(scene.terrain.map_snes,
                            rom.size, &offset));
    data[offset] = 1u;
    for (i = 0u; i < 16u; ++i) {
        const uint16_t word = (uint16_t)(0x0200u + i);
        const size_t position = offset + 0x0120u + i * 2u;
        data[position] = (uint8_t)word;
        data[position + 1u] = (uint8_t)(word >> 8u);
    }

    dk1_vram_image_init(&scene.vram);
    dk1_cgram_init(&scene.cgram);
    scene.cgram.colors[0] = 0x001Fu;
    scene.cgram.colors[1] = 0x7FFFu;
    preset_register(&scene.ppu, 0x2105u, 0x01u);
    preset_register(&scene.ppu, 0x2107u, 0x13u);
    preset_register(&scene.ppu, 0x210Bu, 0x00u);
    preset_register(&scene.ppu, 0x212Cu, 0x01u);

    view = (Dk1SceneView){0u, 0u, 96u, 64u};
    assert(dk1_dynamic_stream_update(
        view, DK1_DYNAMIC_BG1_MAP_COLUMNS, &stream, &update));
    assert(update.changed);
    assert(update.columns_entered == 3u);
    assert(dk1_dynamic_bg1_runtime_init(
        &rom, &scene, &update, &runtime));
    assert(runtime.ready);
    assert(runtime.last.map_columns == 3u);
    assert(runtime.last.tile_columns == 12u);
    assert(runtime.last.tile_words == 768u);
    assert(runtime.update_count == 1u);
    assert(runtime.map_columns_uploaded == 3u);
    assert(runtime.tile_words_uploaded == 768u);
    assert(runtime.vram.bytes[0x2000u] == 0x00u);
    assert(runtime.vram.bytes[0x2001u] == 0x02u);
    first_signature = dk1_dynamic_bg1_runtime_signature(&runtime);
    assert(first_signature != 0u);

    view.camera_x = 32u;
    assert(dk1_dynamic_stream_update(
        view, DK1_DYNAMIC_BG1_MAP_COLUMNS, &stream, &update));
    assert(update.changed);
    assert(update.columns_entered == 1u);
    assert(dk1_dynamic_bg1_runtime_update(
        &rom, &scene, &update, &runtime));
    assert(runtime.last.map_columns == 1u);
    assert(runtime.update_count == 2u);
    assert(runtime.map_columns_uploaded == 4u);
    assert(runtime.tile_words_uploaded == 1024u);
    assert(runtime.generation == 2u);
    assert(dk1_dynamic_bg1_runtime_signature(&runtime) != first_signature);

    memset(pixels, 0, sizeof(pixels));
    surface = (Dk1RgbaSurface){pixels, 8u, 8u, 8u};
    view.width = 8u;
    view.height = 8u;
    assert(dk1_dynamic_bg1_runtime_render(
        &scene, view, &runtime, surface));

    update.changed = false;
    assert(dk1_dynamic_bg1_runtime_update(
        &rom, &scene, &update, &runtime));
    assert(runtime.last.map_columns == 0u);
    assert(runtime.update_count == 2u);
    return 0;
}
