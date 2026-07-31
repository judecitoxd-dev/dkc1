#include <assert.h>
#include <string.h>
#include "dk1/scene_runtime.h"
int main(){Dk1SceneMemory s;Dk1SceneRuntime r;memset(&s,0,sizeof(s));memset(&r,0,sizeof(r));s.camera.maximum_x=1000;r.view.width=384;r.view.height=224;assert(dk1_scene_runtime_step(&s,DK1_RUNTIME_RIGHT,&r)&&r.view.camera_x==4&&r.frame==1);assert(dk1_scene_runtime_step(&s,DK1_RUNTIME_LEFT,&r)&&r.view.camera_x==0);}
