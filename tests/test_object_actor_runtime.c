#include <assert.h>
#include <stdlib.h>
#include "dk1/object_actor_runtime.h"

int main(void){
    const char*path=getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned={0};Dk1RomImage rom;Dk1ObjectActorRuntime actor;
    Dk1VramImage base={{0}};
    Dk1ObjectScreenTransform transform={0,0,0,224,0,0,0};
    Dk1ObjectFrameOamStyle style={0x0040u,0u,3u,false,false};
    bool interaction=true;
    if(!path)path="/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path,&owned,&rom));
    assert(dk1_object_actor_spawn(&actor,&rom,3u,0u,100u,80,
                                  DK1_ACTOR_INTERACTION_NONE));
    assert(actor.active&&actor.scheduler.type_id[3u]==DK1_SIGN_OBJECT_TYPE);
    assert(dk1_object_actor_step(&actor,&rom,&base,transform,style,0u,
                                 100u,80,8,8,&interaction));
    assert(!interaction&&actor.active&&actor.interactions==0u);
    assert(actor.callback_verified&&actor.callback_pc==DK1_SIGN_ACTOR_CALLBACK_PC);
    assert(actor.animation.frame==0x0330u);
    assert(actor.visual_ready&&actor.visual.pieces==12u&&actor.visual.dma_bytes==576u);
    assert(actor.scheduler_callbacks==1u&&actor.frames==1u);
    assert(actor.scheduler.type_id[3u]==DK1_SIGN_OBJECT_TYPE);
    dk1_rom_free(&owned);return 0;
}
