#ifndef DK1_PLAYER_DISPATCH_H
#define DK1_PLAYER_DISPATCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_PLAYER_STATE_COUNT 87u
#define DK1_PLAYER_CALLBACK_BANK 0xBFu
#define DK1_PLAYER_ONE_CALLBACK 0x84ABu
#define DK1_PLAYER_TWO_CALLBACK 0x84A4u

typedef struct Dk1PlayerStateDispatch {
    uint16_t state;
    uint16_t handler_bf;
} Dk1PlayerStateDispatch;

size_t dk1_player_state_count(void);
bool dk1_player_state_dispatch_get(uint16_t state, Dk1PlayerStateDispatch *entry);
uint32_t dk1_player_state_handler_pc(uint16_t state);
uint32_t dk1_player_object_callback_pc(uint8_t player_index);

#endif
