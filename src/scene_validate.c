#include <stdio.h>
#include <stdlib.h>
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"
int main(int argc,char**argv){Dk1OwnedRom o={0};Dk1RomImage r={0};Dk1RomIdentity id;Dk1SceneMemory*s;size_t ok=0,fail=0;uint64_t combined=0;int i;if(argc!=2){fprintf(stderr,"usage: %s ROM\n",argv[0]);return 2;}if(!dk1_rom_load_file(argv[1],&o,&r)||!dk1_rom_identity(&r,&id)||!dk1_rom_identity_is_supported_rev2(&id))return 1;s=malloc(sizeof(*s));if(!s)return 1;for(i=0;i<230;i++){if(dk1_scene_memory_load(&r,(uint16_t)i,false,false,s)){uint64_t h=dk1_scene_memory_signature(s);combined^=h+(uint64_t)i*0x9E3779B97F4A7C15ull;ok++;}else fail++;}printf("scenes=%zu failed=%zu signature=%016llX\n",ok,fail,(unsigned long long)combined);free(s);dk1_rom_free(&o);return fail?1:0;}
