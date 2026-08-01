#ifndef DK1_OBJECT_IDENTITY_H
#define DK1_OBJECT_IDENTITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum Dk1ObjectIdentityKind {
    DK1_OBJECT_IDENTITY_UNKNOWN = 0,
    DK1_OBJECT_IDENTITY_BARREL_FAMILY,
    DK1_OBJECT_IDENTITY_SCENERY
} Dk1ObjectIdentityKind;

typedef struct Dk1ObjectIdentity {
    uint16_t type_id;
    uint32_t callback_pc;
    uint16_t idle_animation_id;
    Dk1ObjectIdentityKind kind;
    const char *name;
} Dk1ObjectIdentity;

#define DK1_OBJECT_TYPE_STEEL_KEG   0x0022u
#define DK1_OBJECT_TYPE_BARREL      0x0023u
#define DK1_OBJECT_TYPE_ROPE_BARREL 0x0024u
#define DK1_OBJECT_TYPE_OIL_DRUM    0x0025u
#define DK1_OBJECT_TYPE_DK_BARREL   0x0026u
#define DK1_OBJECT_TYPE_TNT_BARREL  0x0027u
#define DK1_OBJECT_TYPE_SIGN        0x0073u

#define DK1_COMMON_BARREL_CALLBACK_PC 0xBFCF0Cu
#define DK1_OIL_DRUM_CALLBACK_PC      0xBF83A0u
#define DK1_SIGN_CALLBACK_PC          0xBF8453u

#define DK1_ANIMATION_STEEL_KEG_IDLE   0x00D1u
#define DK1_ANIMATION_BARREL_IDLE      0x00D2u
#define DK1_ANIMATION_ROPE_BARREL_IDLE 0x00D3u
#define DK1_ANIMATION_OIL_DRUM_IDLE    0x00D4u
#define DK1_ANIMATION_DK_BARREL_IDLE   0x00D5u
#define DK1_ANIMATION_TNT_BARREL_IDLE  0x00D6u

size_t dk1_object_identity_count(void);
bool dk1_object_identity_get(uint16_t type_id, Dk1ObjectIdentity *identity);
bool dk1_object_identity_is_barrel_family(uint16_t type_id);

#endif
