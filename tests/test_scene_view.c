#include <assert.h>
#include <string.h>
#include "dk1/scene_view.h"
int main(){Dk1SceneMemory s;Dk1SceneView v={0,600,384,224};memset(&s,0,sizeof(s));s.camera.minimum_x=100;s.camera.maximum_x=1000;assert(dk1_scene_view_clamp(&s,&v));assert(v.camera_x==100&&v.camera_y==288);v.camera_x=900;assert(dk1_scene_view_clamp(&s,&v)&&v.camera_x==616);}
