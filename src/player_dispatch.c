#include "dk1/player_dispatch.h"

/* Exact 87-word jump table at $BF:84CA, selected from $1029. */
static const uint16_t PLAYER_STATE_HANDLERS[DK1_PLAYER_STATE_COUNT] = {
    0x8756u,0x87FDu,0x898Fu,0x8ADBu,0x8BDDu,0x8C0Eu,0x8D00u,0x8D52u,
    0x8D61u,0x8D70u,0x8DFAu,0x8FA7u,0x8FD3u,0x8FD4u,0x8FE0u,0x9009u,
    0x9012u,0x901Bu,0x901Fu,0x9054u,0x9073u,0x9169u,0x9184u,0x9188u,
    0x91A8u,0x91BBu,0x91CEu,0x91E5u,0x922Bu,0x9250u,0x928Bu,0x92AAu,
    0x932Au,0x933Du,0x9365u,0x93FDu,0x94A6u,0x94C7u,0x954Cu,0x961Au,
    0x964Cu,0x9702u,0x9758u,0x9792u,0x97A5u,0x9805u,0x9821u,0x9864u,
    0x9891u,0x990Au,0x999Au,0x99A2u,0x99DBu,0x9A6Bu,0x9AACu,0x9AC9u,
    0x9B60u,0x9B68u,0x9B95u,0x9BA1u,0x9BC9u,0x9BFEu,0x9C33u,0x9C3Fu,
    0x9C57u,0x9C7Eu,0x9CD1u,0x9CD6u,0x9CE4u,0x9CECu,0x9D09u,0x9DE0u,
    0x9DE7u,0x9E15u,0x9E2Fu,0x9E38u,0x9E5Au,0x9E76u,0x9EDBu,0x9F51u,
    0x9F80u,0x9F89u,0x9F90u,0x9FA7u,0x9FF3u,0xA01Eu,0xA023u
};

size_t dk1_player_state_count(void) { return DK1_PLAYER_STATE_COUNT; }

bool dk1_player_state_dispatch_get(uint16_t state, Dk1PlayerStateDispatch *entry) {
    if (entry == NULL || state >= DK1_PLAYER_STATE_COUNT) return false;
    entry->state = state;
    entry->handler_bf = PLAYER_STATE_HANDLERS[state];
    return true;
}

uint32_t dk1_player_state_handler_pc(uint16_t state) {
    if (state >= DK1_PLAYER_STATE_COUNT) return 0u;
    return ((uint32_t)DK1_PLAYER_CALLBACK_BANK << 16u) | PLAYER_STATE_HANDLERS[state];
}

uint32_t dk1_player_object_callback_pc(uint8_t player_index) {
    const uint16_t address = player_index == 1u ? DK1_PLAYER_TWO_CALLBACK :
                             (player_index == 0u ? DK1_PLAYER_ONE_CALLBACK : 0u);
    return address == 0u ? 0u : ((uint32_t)DK1_PLAYER_CALLBACK_BANK << 16u) | address;
}
