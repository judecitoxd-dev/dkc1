#include <assert.h>
#include "dk1/scene_package_plan.h"
int main(){Dk1ScenePackagePlan p;assert(dk1_scene_package_plan(0,1,false,false,&p)&&p.count==4&&p.ids[0]==1&&p.ids[1]==21&&p.ids[3]==20);assert(dk1_scene_package_plan(0xD2,1,true,true,&p)&&p.count==5&&p.ids[1]==22&&p.ids[3]==6);}
