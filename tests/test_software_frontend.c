#include <assert.h>
#include <string.h>
#include "dk1/software_frontend.h"
int main(void) {
    Dk1SceneMemory scene;
    Dk1SoftwareFrontend frontend;
    Dk1Rgba8 pixels[64u * 64u];
    Dk1RgbaSurface surface = {pixels,64u,64u,64u};
    uint64_t before, after;
    memset(&scene,0,sizeof(scene));
    scene.camera.maximum_x=128u;
    scene.cgram.colors[0]=0x001Fu;
    assert(dk1_software_frontend_init(&scene,64u,64u,&frontend));
    before=dk1_software_frontend_signature(&frontend);
    assert(dk1_software_frontend_step(&scene,DK1_HOST_BUTTON_RIGHT|DK1_HOST_BUTTON_R,&frontend));
    after=dk1_software_frontend_signature(&frontend);
    assert(frontend.runtime.view.camera_x==4u);
    assert(frontend.marker_x==34);
    assert(before!=after);
    memset(pixels,0,sizeof(pixels));
    assert(dk1_software_frontend_render(NULL,&scene,&frontend,surface));
    return 0;
}
