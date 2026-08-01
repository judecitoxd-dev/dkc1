#include "dk1/player_combat_runtime.h"
#include <string.h>

void dk1_player_combat_init(Dk1PlayerCombatRuntime *runtime) {
    if (runtime == NULL)
        return;
    memset(runtime, 0, sizeof(*runtime));
}

void dk1_player_combat_step(Dk1PlayerCombatRuntime *runtime) {
    if (runtime == NULL)
        return;
    runtime->hurt_this_frame = false;
    if (runtime->invulnerability_timer > 0u) {
        --runtime->invulnerability_timer;
        ++runtime->invulnerable_frames;
    }
}

bool dk1_player_combat_apply_enemy_hit(Dk1PlayerCombatRuntime *runtime,
                                       Dk1PlayerPreviewRuntime *player,
                                       uint16_t enemy_x) {
    int16_t difference;
    if (runtime == NULL || player == NULL)
        return false;
    if (runtime->invulnerability_timer > 0u) {
        ++runtime->ignored_hits;
        return false;
    }

    difference = (int16_t)(uint16_t)(player->motion.world_x - enemy_x);
    player->motion.velocity_x = difference < 0 ?
        (int16_t)-DK1_PLAYER_HURT_KNOCKBACK_X :
        (int16_t)DK1_PLAYER_HURT_KNOCKBACK_X;
    player->motion.velocity_y = DK1_PLAYER_HURT_KNOCKBACK_Y;
    player->motion.subpixel_x = 0u;
    player->motion.subpixel_y = 0u;
    player->airborne = true;
    player->facing_left = difference >= 0;

    runtime->invulnerability_timer =
        DK1_PLAYER_HURT_INVULNERABILITY_FRAMES;
    ++runtime->accepted_hits;
    runtime->hurt_this_frame = true;
    return true;
}

bool dk1_player_combat_is_invulnerable(
    const Dk1PlayerCombatRuntime *runtime) {
    return runtime != NULL && runtime->invulnerability_timer > 0u;
}

bool dk1_player_combat_should_render(
    const Dk1PlayerCombatRuntime *runtime) {
    if (!dk1_player_combat_is_invulnerable(runtime))
        return true;
    return (runtime->invulnerability_timer & 0x0002u) == 0u;
}
