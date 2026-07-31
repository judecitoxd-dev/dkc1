#include <assert.h>
#include "dk1/save_state.h"
int main(void){Dk1SaveStateInfo a={7,3,0x1122334455667788ull,{{12,34,384,224},99}},b;uint8_t bytes[DK1_SAVE_STATE_BYTES];assert(dk1_save_state_encode(&a,bytes));assert(dk1_save_state_decode(bytes,&b));assert(b.level_id==7&&b.flags==3&&b.scene_signature==a.scene_signature);assert(b.runtime.view.camera_x==12&&b.runtime.frame==99);bytes[21]^=1;assert(!dk1_save_state_decode(bytes,&b));return 0;}
