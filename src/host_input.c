#include "dk1/host_input.h"
#include <ctype.h>
uint16_t dk1_host_key_button(int key){
    key=tolower((unsigned char)key);
    switch(key){
        case 'w': return DK1_HOST_BUTTON_UP; case 's': return DK1_HOST_BUTTON_DOWN;
        case 'a': return DK1_HOST_BUTTON_LEFT; case 'd': return DK1_HOST_BUTTON_RIGHT;
        case 'j': return DK1_HOST_BUTTON_B; case 'k': return DK1_HOST_BUTTON_A;
        case 'u': return DK1_HOST_BUTTON_Y; case 'i': return DK1_HOST_BUTTON_X;
        case 'q': return DK1_HOST_BUTTON_L; case 'e': return DK1_HOST_BUTTON_R;
        case '1': return DK1_HOST_BUTTON_SELECT; case '2': return DK1_HOST_BUTTON_START;
        default:return 0;
    }
}
void dk1_host_input_update(Dk1HostInputState *state,uint16_t held){if(!state)return;state->pressed=(uint16_t)((held^state->held)&held);state->held=held;}
