#include "dk1/scene_signature.h"
uint64_t dk1_fnv1a64(const void*d,size_t n,uint64_t h){const unsigned char*p=d;size_t i;if(h==0)h=1469598103934665603ull;for(i=0;i<n;i++){h^=p[i];h*=1099511628211ull;}return h;}
uint64_t dk1_scene_memory_signature(const Dk1SceneMemory*s){uint64_t h=0;if(!s)return 0;h=dk1_fnv1a64(&s->recipe,sizeof(s->recipe),h);h=dk1_fnv1a64(&s->ppu,sizeof(s->ppu),h);h=dk1_fnv1a64(&s->terrain,sizeof(s->terrain),h);h=dk1_fnv1a64(&s->camera,sizeof(s->camera),h);h=dk1_fnv1a64(s->vram.bytes,sizeof(s->vram.bytes),h);h=dk1_fnv1a64(s->cgram.colors,sizeof(s->cgram.colors),h);return h;}
