#include <stdio.h>
#include "dk1/player_dispatch.h"
#include "dk1/player_state_plan.h"
#include "dk1/rom_image.h"

static uint64_t hash_byte(uint64_t h,uint8_t b){h^=b;return h*1099511628211ULL;}
int main(int argc,char **argv){
 Dk1OwnedRom owned={0}; Dk1RomImage rom={0}; Dk1RomIdentity id; uint64_t h=1469598103934665603ULL;
 size_t state,i; unsigned invalid=0,planned=0;
 if(argc!=2){fprintf(stderr,"usage: %s ROM\n",argv[0]);return 2;}
 if(!dk1_rom_load_file(argv[1],&owned,&rom)||!dk1_rom_identity(&rom,&id)||!dk1_rom_identity_is_supported_rev2(&id)){fputs("unsupported ROM\n",stderr);return 1;}
 for(state=0;state<dk1_player_state_count();++state){
  Dk1PlayerStateDispatch e; Dk1PlayerStatePlan p; uint8_t byte=0;
  if(!dk1_player_state_dispatch_get((uint16_t)state,&e)||!dk1_rom_read_u8(&rom,0xBF0000u|e.handler_bf,&byte)){++invalid;continue;}
  h=hash_byte(h,(uint8_t)state);h=hash_byte(h,(uint8_t)e.handler_bf);h=hash_byte(h,(uint8_t)(e.handler_bf>>8));h=hash_byte(h,byte);
  if(dk1_player_state_plan_get((uint16_t)state,&p)){++planned;for(i=0;i<p.step_count;++i)h=hash_byte(h,(uint8_t)p.steps[i].source_pc);}
 }
 printf("player_states=%u invalid=%u wrapper_plans=%u signature=%016llX\n",(unsigned)dk1_player_state_count(),invalid,planned,(unsigned long long)h);
 dk1_rom_free(&owned);return invalid==0?0:1;
}
