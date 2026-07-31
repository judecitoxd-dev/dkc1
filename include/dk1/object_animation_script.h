#ifndef DK1_OBJECT_ANIMATION_SCRIPT_H
#define DK1_OBJECT_ANIMATION_SCRIPT_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_ANIMATION_SCRIPT_BANK 0xBEu
#define DK1_ANIMATION_POINTER_TABLE 0xBE8572u
#define DK1_ANIMATION_MAX_COMMANDS_PER_TICK 64u

typedef struct Dk1ObjectAnimationScriptState {
    uint16_t animation_id;       /* $10D1 */
    int16_t accumulator;         /* $1105 */
    int16_t speed;               /* $1139 */
    uint16_t script_pointer;     /* $116D, bank $BE */
    uint16_t frame;              /* $0D11 */
    uint16_t callback_address;  /* $1341 */
    uint8_t callback_bank;       /* low byte of $130D */
} Dk1ObjectAnimationScriptState;

typedef enum Dk1AnimationScriptStatus {
    DK1_ANIMATION_IDLE = 0,
    DK1_ANIMATION_FRAME_READY,
    DK1_ANIMATION_EXTERNAL_CALL,
    DK1_ANIMATION_UNSUPPORTED_COMMAND,
    DK1_ANIMATION_INVALID_DATA
} Dk1AnimationScriptStatus;

typedef struct Dk1AnimationScriptResult {
    Dk1AnimationScriptStatus status;
    uint8_t command;
    uint8_t event_kind;
    uint8_t event_value;
    uint32_t external_pc;
    uint32_t callback_pc;
    uint16_t commands_processed;
    bool frame_changed;
    bool callback_changed;
} Dk1AnimationScriptResult;

bool dk1_animation_script_pointer(const Dk1RomImage *rom, uint16_t animation_id,
                                  uint16_t *pointer);

/* Clean-room translation of the normal-frame path at $BE:80E1 plus common commands
 * $80/$91 (restart from animation table), $81/$83 (external call), $82 (jump),
 * $84 (set object callback), and $8E-$90 (one-byte event commands).
 * More complex paired-object commands remain explicit unsupported results. */
bool dk1_object_animation_step(const Dk1RomImage *rom,
                               Dk1ObjectAnimationScriptState *state,
                               Dk1AnimationScriptResult *result);

#endif
