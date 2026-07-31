#include <assert.h>
#include <string.h>
#include "dk1/scene_signature.h"
int main(){Dk1SceneMemory s;uint64_t a,b;memset(&s,0,sizeof(s));a=dk1_scene_memory_signature(&s);s.cgram.colors[1]=1;b=dk1_scene_memory_signature(&s);assert(a&&b&&a!=b);assert(dk1_fnv1a64("abc",3,0)==0xE16801510DB89EFDull);}
