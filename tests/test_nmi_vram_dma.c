#include <assert.h>
#include <string.h>
#include "dk1/nmi_vram_dma.h"
typedef struct Mem{uint8_t a[256];}Mem;static bool rd(void*c,uint32_t pc,uint8_t*out,size_t n){Mem*m=c;uint16_t a=(uint16_t)pc;if((size_t)a+n>sizeof(m->a))return false;memcpy(out,m->a+a,n);return true;}
int main(void){Dk1NmiVramDmaQueue q;Dk1VramImage v;Dk1NmiVramDmaStats s;Mem m;size_t i;for(i=0;i<256;i++)m.a[i]=(uint8_t)(i^0x5a);memset(&v,0,sizeof(v));dk1_nmi_vram_dma_init(&q);assert(dk1_nmi_vram_dma_push(&q,8,0x0010,0x7E0020));assert(dk1_nmi_vram_dma_push(&q,6,0x7FFF,0x7E0040));assert(q.records[0].source_bank_active==0x807e);assert(dk1_nmi_vram_dma_execute(&q,rd,&m,&v,&s));assert(s.records==2&&s.bytes==14);for(i=0;i<8;i++)assert(v.bytes[0x20+i]==m.a[0x20+i]);assert(v.bytes[0xfffe]==m.a[0x40]&&v.bytes[0xffff]==m.a[0x41]&&v.bytes[0]==m.a[0x42]);assert(q.records[0].source_bank_active==0&&q.records[1].source_bank_active==0);return 0;}
