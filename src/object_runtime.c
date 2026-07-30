#include "dk1/object_runtime.h"

static const Dk1ObjectTypeDispatch OBJECT_TYPES[DK1_OBJECT_TYPE_COUNT] = {
    {0x84A3u, 0x0030u}, /*   0 */
    {0x84ABu, 0x00F0u}, /*   1 */
    {0x84A4u, 0x00F0u}, /*   2 */
    {0xC697u, 0x0030u}, /*   3 */
    {0xC707u, 0x0030u}, /*   4 */
    {0xF15Au, 0x0030u}, /*   5 */
    {0xC869u, 0x0030u}, /*   6 */
    {0xEFE1u, 0x0070u}, /*   7 */
    {0xC940u, 0x0030u}, /*   8 */
    {0xCB38u, 0x0030u}, /*   9 */
    {0xCB38u, 0x0030u}, /*  10 */
    {0xCB38u, 0x0030u}, /*  11 */
    {0xCB38u, 0x0030u}, /*  12 */
    {0x8378u, 0x0030u}, /*  13 */
    {0x83E4u, 0x0030u}, /*  14 */
    {0xEFDAu, 0x0090u}, /*  15 */
    {0xDA99u, 0x0020u}, /*  16 */
    {0xD967u, 0x0030u}, /*  17 */
    {0xD993u, 0x0030u}, /*  18 */
    {0xD9E9u, 0x0030u}, /*  19 */
    {0x8390u, 0x0030u}, /*  20 */
    {0xD7A3u, 0x0020u}, /*  21 */
    {0xD6CFu, 0x00A0u}, /*  22 */
    {0xD4DBu, 0x00D0u}, /*  23 */
    {0xD3E1u, 0x0030u}, /*  24 */
    {0xE136u, 0x0030u}, /*  25 */
    {0xE0C4u, 0x0030u}, /*  26 */
    {0xDD2Cu, 0x0030u}, /*  27 */
    {0xDD2Cu, 0x0030u}, /*  28 */
    {0xDEBBu, 0x0030u}, /*  29 */
    {0xDEBBu, 0x0030u}, /*  30 */
    {0xDD2Cu, 0x0030u}, /*  31 */
    {0xDA4Bu, 0x0030u}, /*  32 */
    {0xDE2Bu, 0x0030u}, /*  33 */
    {0xCF0Cu, 0x0030u}, /*  34 */
    {0xCF0Cu, 0x0030u}, /*  35 */
    {0xCF0Cu, 0x0030u}, /*  36 */
    {0x83A0u, 0x0030u}, /*  37 */
    {0xCF0Cu, 0x0070u}, /*  38 */
    {0xCF0Cu, 0x0030u}, /*  39 */
    {0x839Cu, 0x0030u}, /*  40 */
    {0xC7EBu, 0x0030u}, /*  41 */
    {0x83ACu, 0x00D0u}, /*  42 */
    {0xCA1Bu, 0x0070u}, /*  43 */
    {0xE189u, 0x0030u}, /*  44 */
    {0xE291u, 0x0030u}, /*  45 */
    {0x83A8u, 0x0010u}, /*  46 */
    {0xF035u, 0x0030u}, /*  47 */
    {0xEE60u, 0x0030u}, /*  48 */
    {0xECD1u, 0x00B0u}, /*  49 */
    {0xECCAu, 0x0050u}, /*  50 */
    {0xE3A2u, 0x0030u}, /*  51 */
    {0x83F0u, 0x0030u}, /*  52 */
    {0xE296u, 0x0030u}, /*  53 */
    {0x8414u, 0x0030u}, /*  54 */
    {0xECCAu, 0x0010u}, /*  55 */
    {0xE3ABu, 0x0030u}, /*  56 */
    {0xEA94u, 0x0030u}, /*  57 */
    {0xEA7Bu, 0x0030u}, /*  58 */
    {0xEFD3u, 0x0070u}, /*  59 */
    {0xEFCCu, 0x0030u}, /*  60 */
    {0xEA80u, 0x0030u}, /*  61 */
    {0x8418u, 0x0030u}, /*  62 */
    {0x83CCu, 0x0030u}, /*  63 */
    {0xFD64u, 0x0030u}, /*  64 */
    {0xD50Cu, 0x0070u}, /*  65 */
    {0xD4DFu, 0x0070u}, /*  66 */
    {0x83B0u, 0x0010u}, /*  67 */
    {0x845Bu, 0x0030u}, /*  68 */
    {0x83F4u, 0x0030u}, /*  69 */
    {0x83F8u, 0x0030u}, /*  70 */
    {0xEA80u, 0x0030u}, /*  71 */
    {0x83FCu, 0x0030u}, /*  72 */
    {0x8400u, 0x0030u}, /*  73 */
    {0x8374u, 0x0070u}, /*  74 */
    {0x8404u, 0x0030u}, /*  75 */
    {0x8408u, 0x0030u}, /*  76 */
    {0x840Cu, 0x0030u}, /*  77 */
    {0x8410u, 0x0030u}, /*  78 */
    {0x841Cu, 0x0030u}, /*  79 */
    {0x8420u, 0x0030u}, /*  80 */
    {0x8424u, 0x0030u}, /*  81 */
    {0x83ECu, 0x0030u}, /*  82 */
    {0x8428u, 0x0030u}, /*  83 */
    {0x842Cu, 0x0030u}, /*  84 */
    {0x8430u, 0x0030u}, /*  85 */
    {0x8434u, 0x0030u}, /*  86 */
    {0x8438u, 0x0030u}, /*  87 */
    {0x843Cu, 0x0030u}, /*  88 */
    {0x8440u, 0x0010u}, /*  89 */
    {0x83E8u, 0x0030u}, /*  90 */
    {0x84A3u, 0x0030u}, /*  91 */
    {0x83E0u, 0x0030u}, /*  92 */
    {0x83DCu, 0x0030u}, /*  93 */
    {0x83D8u, 0x0030u}, /*  94 */
    {0x83D0u, 0x0030u}, /*  95 */
    {0x83D4u, 0x0030u}, /*  96 */
    {0x83C8u, 0x0030u}, /*  97 */
    {0x83C4u, 0x0010u}, /*  98 */
    {0x83C0u, 0x0030u}, /*  99 */
    {0x83BCu, 0x0030u}, /* 100 */
    {0x83B8u, 0x00B0u}, /* 101 */
    {0xFD54u, 0x0010u}, /* 102 */
    {0xC938u, 0x0090u}, /* 103 */
    {0x83A4u, 0x0030u}, /* 104 */
    {0x83B4u, 0x00B0u}, /* 105 */
    {0x8398u, 0x0030u}, /* 106 */
    {0x8394u, 0x0030u}, /* 107 */
    {0x8384u, 0x0030u}, /* 108 */
    {0x8388u, 0x0030u}, /* 109 */
    {0x838Cu, 0x0030u}, /* 110 */
    {0x8380u, 0x00B0u}, /* 111 */
    {0x837Cu, 0x0030u}, /* 112 */
    {0x8444u, 0x0030u}, /* 113 */
    {0x8370u, 0x00B0u}, /* 114 */
    {0x8453u, 0x0030u}, /* 115 */
    {0x836Cu, 0x00B0u}, /* 116 */
    {0x8448u, 0x00B0u}, /* 117 */
    {0x8368u, 0x0030u}, /* 118 */
    {0x8364u, 0x0030u}, /* 119 */
    {0x8360u, 0x0030u}, /* 120 */
    {0xC05Cu, 0xB685u}, /* 121 */
};

uint16_t dk1_object_slot_offset(size_t slot) {
    return slot < DK1_OBJECT_SLOT_COUNT ? (uint16_t)(slot * 2u) : UINT16_MAX;
}

size_t dk1_object_pool_first_slot(Dk1ObjectPool pool) {
    return pool == DK1_OBJECT_POOL_SECONDARY ? DK1_SECONDARY_OBJECT_FIRST_SLOT : DK1_PRIMARY_OBJECT_FIRST_SLOT;
}

size_t dk1_object_pool_slot_count(Dk1ObjectPool pool) {
    return pool == DK1_OBJECT_POOL_SECONDARY ? DK1_SECONDARY_OBJECT_SLOT_COUNT : DK1_PRIMARY_OBJECT_SLOT_COUNT;
}

bool dk1_object_slot_in_pool(size_t slot, Dk1ObjectPool pool) {
    const size_t first = dk1_object_pool_first_slot(pool);
    const size_t count = dk1_object_pool_slot_count(pool);
    return slot >= first && slot < first + count;
}

bool dk1_object_callback_is_set(Dk1ObjectCallback callback) {
    return (callback.bank_field & 0x00FFu) != 0u && callback.address != 0u;
}

uint32_t dk1_object_callback_pc(Dk1ObjectCallback callback) {
    return ((uint32_t)(callback.bank_field & 0x00FFu) << 16u) | callback.address;
}

Dk1ObjectCallback dk1_object_callback_from_script(uint16_t address, uint8_t bank) {
    const Dk1ObjectCallback callback = {address, bank};
    return callback;
}

size_t dk1_object_type_count(void) { return DK1_OBJECT_TYPE_COUNT; }

bool dk1_object_type_dispatch_get(uint16_t type_id, Dk1ObjectTypeDispatch *entry) {
    if (entry == NULL || type_id >= DK1_OBJECT_TYPE_COUNT) return false;
    *entry = OBJECT_TYPES[type_id];
    return true;
}

uint8_t dk1_object_type_priority(Dk1ObjectTypeDispatch entry) {
    return (uint8_t)(entry.attributes & 0x003Fu);
}

bool dk1_object_type_special_pass(Dk1ObjectTypeDispatch entry) {
    return (entry.attributes & 0x0080u) != 0u;
}
