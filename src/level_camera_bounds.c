#include "dk1/level_camera_bounds.h"
bool dk1_level_camera_bounds_read(const Dk1RomImage*r,uint16_t id,Dk1LevelCameraBounds*b){uint16_t p=0,base;if(!r||!b||id>=230||!dk1_rom_read_u16(r,0xBC8000u+(uint32_t)id*2u,&p)||p<4)return false;base=(uint16_t)(p-4u);return dk1_rom_read_u16(r,0xBC0000u+base,&b->minimum_x)&&dk1_rom_read_u16(r,0xBC0002u+base,&b->maximum_x);}
