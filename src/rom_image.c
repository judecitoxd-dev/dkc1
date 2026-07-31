#include "dk1/rom_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool dk1_rom_image_init(Dk1RomImage *image, const uint8_t *data, size_t size) {
    if (image == NULL || data == NULL || size == 0u) return false;
    image->data = data;
    image->size = size;
    return true;
}

bool dk1_rom_load_file(const char *path, Dk1OwnedRom *owned, Dk1RomImage *image) {
    FILE *file = NULL;
    long length = 0;
    size_t read_size = 0u;
    uint8_t *data = NULL;
    if (path == NULL || owned == NULL || image == NULL) return false;
    owned->data = NULL;
    owned->size = 0u;
    file = fopen(path, "rb");
    if (file == NULL) return false;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return false; }
    length = ftell(file);
    if (length <= 0 || fseek(file, 0, SEEK_SET) != 0) { fclose(file); return false; }
    data = (uint8_t *)malloc((size_t)length);
    if (data == NULL) { fclose(file); return false; }
    read_size = fread(data, 1u, (size_t)length, file);
    fclose(file);
    if (read_size != (size_t)length) { free(data); return false; }
    owned->data = data;
    owned->size = read_size;
    image->data = data;
    image->size = read_size;
    return true;
}

void dk1_rom_free(Dk1OwnedRom *owned) {
    if (owned == NULL) return;
    free(owned->data);
    owned->data = NULL;
    owned->size = 0u;
}

bool dk1_hirom_offset(uint32_t snes_pc, size_t rom_size, size_t *offset) {
    const uint8_t bank = (uint8_t)(snes_pc >> 16u);
    const uint16_t address = (uint16_t)snes_pc;
    const bool full_bank_mapping = (bank >= 0x40u && bank <= 0x7Du) || bank >= 0xC0u;
    size_t candidate = 0u;
    if (offset == NULL || (!full_bank_mapping && address < 0x8000u)) return false;
    candidate = ((size_t)(bank & 0x3Fu) << 16u) | address;
    if (candidate >= rom_size) return false;
    *offset = candidate;
    return true;
}

bool dk1_rom_read_u8(const Dk1RomImage *image, uint32_t snes_pc, uint8_t *value) {
    size_t offset = 0u;
    if (image == NULL || value == NULL || image->data == NULL || !dk1_hirom_offset(snes_pc, image->size, &offset)) return false;
    *value = image->data[offset];
    return true;
}

bool dk1_rom_copy(const Dk1RomImage *image, uint32_t snes_pc, void *output, size_t size) {
    size_t offset = 0u;
    if (image == NULL || image->data == NULL || output == NULL || size == 0u || !dk1_hirom_offset(snes_pc, image->size, &offset) || size > image->size - offset) return false;
    memcpy(output, image->data + offset, size);
    return true;
}

bool dk1_rom_read_u16(const Dk1RomImage *image, uint32_t snes_pc, uint16_t *value) {
    uint8_t bytes[2];
    if (value == NULL || !dk1_rom_copy(image, snes_pc, bytes, sizeof(bytes))) return false;
    *value = (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8u));
    return true;
}

bool dk1_rom_read_u24(const Dk1RomImage *image, uint32_t snes_pc, uint32_t *value) {
    uint8_t bytes[3];
    if (value == NULL || !dk1_rom_copy(image, snes_pc, bytes, sizeof(bytes))) return false;
    *value = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8u) | ((uint32_t)bytes[2] << 16u);
    return true;
}

bool dk1_rom_identity(const Dk1RomImage *image, Dk1RomIdentity *identity) {
    const uint8_t *header = NULL;
    if (image == NULL || identity == NULL || image->data == NULL || image->size < DK1_HIROM_HEADER_OFFSET + 0x20u) return false;
    header = image->data + DK1_HIROM_HEADER_OFFSET;
    memcpy(identity->title, header, 21u);
    identity->title[21] = '\0';
    identity->map_mode = header[0x15];
    identity->cartridge_type = header[0x16];
    identity->rom_size_code = header[0x17];
    identity->sram_size_code = header[0x18];
    identity->region_code = header[0x19];
    identity->version = header[0x1Bu];
    identity->checksum_complement = (uint16_t)(header[0x1Cu] | ((uint16_t)header[0x1Du] << 8u));
    identity->checksum = (uint16_t)(header[0x1Eu] | ((uint16_t)header[0x1Fu] << 8u));
    return true;
}

bool dk1_rom_identity_is_supported_rev2(const Dk1RomIdentity *identity) {
    static const char title[22] = "DONKEY KONG COUNTRY  ";
    return identity != NULL && memcmp(identity->title, title, 21u) == 0 && identity->map_mode == 0x31u && identity->cartridge_type == 0x02u && identity->rom_size_code == 0x0Cu && identity->sram_size_code == 0x01u && identity->region_code == 0x01u && identity->version == 0x02u && (uint16_t)(identity->checksum + identity->checksum_complement) == 0xFFFFu;
}
