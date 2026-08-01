#include <assert.h>
#include <string.h>
#include "dk1/object_identity.h"

int main(void) {
    Dk1ObjectIdentity id;
    assert(dk1_object_identity_count() == 8u);
    assert(dk1_object_identity_get(DK1_OBJECT_TYPE_SIGN, &id));
    assert(id.callback_pc == DK1_SIGN_CALLBACK_PC);
    assert(id.kind == DK1_OBJECT_IDENTITY_SCENERY);
    assert(strcmp(id.name, "Sign") == 0);
    assert(dk1_object_identity_get(DK1_OBJECT_TYPE_BARREL, &id));
    assert(id.callback_pc == DK1_COMMON_BARREL_CALLBACK_PC);
    assert(id.idle_animation_id == DK1_ANIMATION_BARREL_IDLE);
    assert(strcmp(id.name, "Barrel") == 0);
    assert(dk1_object_identity_get(DK1_OBJECT_TYPE_GNAWTY, &id));
    assert(id.callback_pc == DK1_GNAWTY_CALLBACK_PC);
    assert(id.idle_animation_id == DK1_ANIMATION_GNAWTY_WALK);
    assert(id.kind == DK1_OBJECT_IDENTITY_ENEMY);
    assert(strcmp(id.name, "Gnawty") == 0);
    assert(dk1_object_identity_is_enemy(DK1_OBJECT_TYPE_GNAWTY));
    assert(!dk1_object_identity_is_enemy(DK1_OBJECT_TYPE_SIGN));
    assert(dk1_object_identity_is_barrel_family(DK1_OBJECT_TYPE_STEEL_KEG));
    assert(dk1_object_identity_is_barrel_family(DK1_OBJECT_TYPE_TNT_BARREL));
    assert(!dk1_object_identity_is_barrel_family(DK1_OBJECT_TYPE_SIGN));
    assert(!dk1_object_identity_get(0xFFFFu, &id));
    return 0;
}
