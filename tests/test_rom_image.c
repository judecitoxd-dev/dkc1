#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/rom_image.h"

int main(void) {
    uint8_t *bytes = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1RomImage image;
    Dk1RomIdentity identity;
    size_t offset = 0u;
    uint16_t word = 0u;
    uint8_t *header;
    assert(bytes != NULL);
    header = bytes + DK1_HIROM_HEADER_OFFSET;
    memcpy(header, "DONKEY KONG COUNTRY  ", 21u);
    header[0x15] = 0x31u;
    header[0x16] = 0x02u;
    header[0x17] = 0x0Cu;
    header[0x18] = 0x01u;
    header[0x19] = 0x01u;
    header[0x1B] = 0x02u;
    header[0x1C] = 0x33u;
    header[0x1D] = 0xD4u;
    header[0x1E] = 0xCCu;
    header[0x1F] = 0x2Bu;
    bytes[0x008000] = 0x34u;
    bytes[0x008001] = 0x12u;

    assert(dk1_rom_image_init(&image, bytes, DK1_EXPECTED_ROM_SIZE));
    assert(dk1_hirom_offset(0x008000u, image.size, &offset));
    assert(offset == 0x008000u);
    assert(dk1_hirom_offset(0xC00000u, image.size, &offset));
    assert(offset == 0u);
    assert(!dk1_hirom_offset(0x001000u, image.size, &offset));
    assert(dk1_rom_read_u16(&image, 0x008000u, &word));
    assert(word == 0x1234u);
    assert(dk1_rom_identity(&image, &identity));
    assert(dk1_rom_identity_is_supported_rev2(&identity));
    identity.version = 1u;
    assert(!dk1_rom_identity_is_supported_rev2(&identity));
    free(bytes);
    return 0;
}
