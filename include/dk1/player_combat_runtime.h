#ifndef DK1_PLAYER_COMBAT_RUNTIME_H
#define DK1_PLAYER_COMBAT_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/player_preview_runtime.h"

#define DK1_PLAYER_HURT_INVULNERABILITY_FRAMES 90u
#define DK1_PLAYER_HURT_KNOCKBACK_X 0x0180
#define DK1_PLAYER_HURT_KNOCKBACK_Y 0x0300

typedef struct Dk1PlayerCombatRuntime {
    uint16_t invulnerability_timer;
    uint32_t accepted_hits;
    uint32_t ignored_hits;
    uint32_t invulnerable_frames;
    bool hurt_this_frame;
} Dk1PlayerCombatRuntime;

void dk1_player_combat_init(Dk1PlayerCombatRuntime *runtime);
void dk1_player_combat_step(Dk1PlayerCombatRuntime *runtime);

/* Portable frontend bridge until the original shared damage helper, Kong swap,
 * hurt states and invulnerability flags are translated. Returns true only when
 * a new hit is accepted. */
bool dk1_player_combat_apply_enemy_hit(Dk1PlayerCombatRuntime *runtime,
                                       Dk1PlayerPreviewRuntime *player,
                                       uint16_t enemy_x);

bool dk1_player_combat_is_invulnerable(
    const Dk1PlayerCombatRuntime *runtime);
bool dk1_player_combat_should_render(
    const Dk1PlayerCombatRuntime *runtime);

#endif
