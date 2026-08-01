#include "dk1/object_identity.h"

static const Dk1ObjectIdentity IDENTITIES[] = {
    {DK1_OBJECT_TYPE_STEEL_KEG, DK1_COMMON_BARREL_CALLBACK_PC,
     DK1_ANIMATION_STEEL_KEG_IDLE, DK1_OBJECT_IDENTITY_BARREL_FAMILY,
     "Steel Keg"},
    {DK1_OBJECT_TYPE_BARREL, DK1_COMMON_BARREL_CALLBACK_PC,
     DK1_ANIMATION_BARREL_IDLE, DK1_OBJECT_IDENTITY_BARREL_FAMILY,
     "Barrel"},
    {DK1_OBJECT_TYPE_ROPE_BARREL, DK1_COMMON_BARREL_CALLBACK_PC,
     DK1_ANIMATION_ROPE_BARREL_IDLE, DK1_OBJECT_IDENTITY_BARREL_FAMILY,
     "Rope Barrel"},
    {DK1_OBJECT_TYPE_OIL_DRUM, DK1_OIL_DRUM_CALLBACK_PC,
     DK1_ANIMATION_OIL_DRUM_IDLE, DK1_OBJECT_IDENTITY_BARREL_FAMILY,
     "Oil Drum"},
    {DK1_OBJECT_TYPE_DK_BARREL, DK1_COMMON_BARREL_CALLBACK_PC,
     DK1_ANIMATION_DK_BARREL_IDLE, DK1_OBJECT_IDENTITY_BARREL_FAMILY,
     "DK Barrel"},
    {DK1_OBJECT_TYPE_TNT_BARREL, DK1_COMMON_BARREL_CALLBACK_PC,
     DK1_ANIMATION_TNT_BARREL_IDLE, DK1_OBJECT_IDENTITY_BARREL_FAMILY,
     "TNT Barrel"},
    {DK1_OBJECT_TYPE_SIGN, DK1_SIGN_CALLBACK_PC, 0u,
     DK1_OBJECT_IDENTITY_SCENERY, "Sign"},
};

size_t dk1_object_identity_count(void) {
    return sizeof(IDENTITIES) / sizeof(IDENTITIES[0]);
}

bool dk1_object_identity_get(uint16_t type_id, Dk1ObjectIdentity *identity) {
    size_t i;
    if (identity == NULL) return false;
    for (i = 0u; i < dk1_object_identity_count(); ++i) {
        if (IDENTITIES[i].type_id == type_id) {
            *identity = IDENTITIES[i];
            return true;
        }
    }
    return false;
}

bool dk1_object_identity_is_barrel_family(uint16_t type_id) {
    Dk1ObjectIdentity identity;
    return dk1_object_identity_get(type_id, &identity) &&
           identity.kind == DK1_OBJECT_IDENTITY_BARREL_FAMILY;
}
