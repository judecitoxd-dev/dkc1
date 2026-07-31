#include "dk1/save_state.h"
#include <stdio.h>
#include <string.h>

static void write16(uint8_t *p, uint16_t v) { p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8u); }
static void write64(uint8_t *p, uint64_t v) { unsigned i; for(i=0;i<8u;i++)p[i]=(uint8_t)(v>>(i*8u)); }
static uint16_t read16(const uint8_t *p) { return (uint16_t)p[0]|((uint16_t)p[1]<<8u); }
static uint64_t read64(const uint8_t *p) { uint64_t v=0; unsigned i; for(i=0;i<8u;i++)v|=(uint64_t)p[i]<<(i*8u); return v; }
static uint32_t checksum(const uint8_t *p, size_t n) { uint32_t h=2166136261u; size_t i; for(i=0;i<n;i++){h^=p[i];h*=16777619u;}return h; }
static void write32(uint8_t *p,uint32_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8u);p[2]=(uint8_t)(v>>16u);p[3]=(uint8_t)(v>>24u);}
static uint32_t read32(const uint8_t*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8u)|((uint32_t)p[2]<<16u)|((uint32_t)p[3]<<24u);}

bool dk1_save_state_encode(const Dk1SaveStateInfo *state,
                           uint8_t output[DK1_SAVE_STATE_BYTES]) {
    if (state == NULL || output == NULL) return false;
    memset(output, 0, DK1_SAVE_STATE_BYTES);
    memcpy(output, "DK1S", 4u);
    write16(output+4u,1u); write16(output+6u,state->level_id); write16(output+8u,state->flags);
    write64(output+12u,state->scene_signature);
    write16(output+20u,state->runtime.view.camera_x); write16(output+22u,state->runtime.view.camera_y);
    write16(output+24u,state->runtime.view.width); write16(output+26u,state->runtime.view.height);
    write64(output+28u,state->runtime.frame);
    write32(output+44u,checksum(output,44u));
    return true;
}

bool dk1_save_state_decode(const uint8_t input[DK1_SAVE_STATE_BYTES],
                           Dk1SaveStateInfo *state) {
    if (input == NULL || state == NULL || memcmp(input,"DK1S",4u)!=0 || read16(input+4u)!=1u ||
        read32(input+44u)!=checksum(input,44u)) return false;
    memset(state,0,sizeof(*state));
    state->level_id=read16(input+6u); state->flags=read16(input+8u); state->scene_signature=read64(input+12u);
    state->runtime.view.camera_x=read16(input+20u); state->runtime.view.camera_y=read16(input+22u);
    state->runtime.view.width=read16(input+24u); state->runtime.view.height=read16(input+26u);
    state->runtime.frame=read64(input+28u);
    return true;
}

bool dk1_save_state_write_file(const char *path, const Dk1SaveStateInfo *state) {
    uint8_t bytes[DK1_SAVE_STATE_BYTES]; FILE *f;
    if(path==NULL||!dk1_save_state_encode(state,bytes))return false;
    f=fopen(path,"wb"); if(f==NULL)return false;
    if(fwrite(bytes,1,sizeof(bytes),f)!=sizeof(bytes)){fclose(f);return false;} return fclose(f)==0;
}

bool dk1_save_state_read_file(const char *path, Dk1SaveStateInfo *state) {
    uint8_t bytes[DK1_SAVE_STATE_BYTES]; FILE *f; size_t n;
    if (path == NULL || state == NULL) return false;
    f = fopen(path, "rb");
    if (f == NULL) return false;
    n = fread(bytes, 1, sizeof(bytes), f);
    if (fclose(f) != 0 || n != sizeof(bytes)) return false;
    return dk1_save_state_decode(bytes,state);
}
