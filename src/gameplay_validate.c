#include <stdio.h>
#include "dk1/apu_boot_image.h"
#include "dk1/player_coverage.h"
#include "dk1/player_dispatch.h"
#include "dk1/rom_image.h"

int main(int argc,char **argv){Dk1OwnedRom owned={0};Dk1RomImage rom={0};Dk1RomIdentity id;Dk1ApuBootImage apu;unsigned invalid=0;uint16_t s;if(argc!=2){fprintf(stderr,"usage: %s ROM\n",argv[0]);return 2;}if(!dk1_rom_load_file(argv[1],&owned,&rom)||!dk1_rom_identity(&rom,&id)||!dk1_rom_identity_is_supported_rev2(&id)||!dk1_apu_boot_image_read(&rom,&apu)){fputs("unsupported ROM\n",stderr);return 1;}for(s=0;s<DK1_PLAYER_STATE_COUNT;s++){uint8_t opcode;if(dk1_player_state_translation_level(s)!=DK1_PLAYER_STATE_UNTRANSLATED&&!dk1_rom_read_u8(&rom,dk1_player_state_handler_pc(s),&opcode))invalid++;}printf("player_states=%u planned=%u local=%u invalid=%u translation=%016llX apu_boot=%016llX\n",(unsigned)DK1_PLAYER_STATE_COUNT,(unsigned)dk1_player_planned_state_count(),(unsigned)dk1_player_translated_state_count(),invalid,(unsigned long long)dk1_player_translation_signature(),(unsigned long long)apu.signature);dk1_rom_free(&owned);return invalid?1:0;}
