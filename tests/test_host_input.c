#include <assert.h>
#include "dk1/host_input.h"
int main(void){Dk1HostInputState s={0};assert(dk1_host_key_button('w')==DK1_HOST_BUTTON_UP);assert(dk1_host_key_button('D')==DK1_HOST_BUTTON_RIGHT);dk1_host_input_update(&s,DK1_HOST_BUTTON_B);assert(s.pressed==DK1_HOST_BUTTON_B);dk1_host_input_update(&s,DK1_HOST_BUTTON_B|DK1_HOST_BUTTON_RIGHT);assert(s.pressed==DK1_HOST_BUTTON_RIGHT);return 0;}
