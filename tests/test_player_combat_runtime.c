#include <assert.h>
#include "dk1/player_combat_runtime.h"

int main(void) {
    Dk1PlayerCombatRuntime combat;
    Dk1PlayerPreviewRuntime player;
    unsigned i;

    dk1_player_preview_init(&player, 100u, 80);
    dk1_player_combat_init(&combat);
    assert(!dk1_player_combat_is_invulnerable(&combat));
    assert(dk1_player_combat_should_render(&combat));

    assert(dk1_player_combat_apply_enemy_hit(&combat, &player, 120u));
    assert(combat.accepted_hits == 1u);
    assert(combat.ignored_hits == 0u);
    assert(combat.hurt_this_frame);
    assert(combat.invulnerability_timer ==
           DK1_PLAYER_HURT_INVULNERABILITY_FRAMES);
    assert(player.motion.velocity_x == -DK1_PLAYER_HURT_KNOCKBACK_X);
    assert(player.motion.velocity_y == DK1_PLAYER_HURT_KNOCKBACK_Y);
    assert(player.airborne);
    assert(!player.facing_left);

    assert(!dk1_player_combat_apply_enemy_hit(&combat, &player, 80u));
    assert(combat.accepted_hits == 1u);
    assert(combat.ignored_hits == 1u);

    dk1_player_combat_step(&combat);
    assert(!combat.hurt_this_frame);
    assert(combat.invulnerability_timer ==
           DK1_PLAYER_HURT_INVULNERABILITY_FRAMES - 1u);
    assert(combat.invulnerable_frames == 1u);

    for (i = 1u; i < DK1_PLAYER_HURT_INVULNERABILITY_FRAMES; ++i)
        dk1_player_combat_step(&combat);
    assert(!dk1_player_combat_is_invulnerable(&combat));
    assert(dk1_player_combat_should_render(&combat));

    player.motion.world_x = 140u;
    assert(dk1_player_combat_apply_enemy_hit(&combat, &player, 120u));
    assert(player.motion.velocity_x == DK1_PLAYER_HURT_KNOCKBACK_X);
    assert(player.facing_left);
    assert(combat.accepted_hits == 2u);
    return 0;
}
