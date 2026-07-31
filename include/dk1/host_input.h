#ifndef DK1_HOST_INPUT_H
#define DK1_HOST_INPUT_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/scene_runtime.h"
#define DK1_HOST_BUTTON_B 0x8000u
#define DK1_HOST_BUTTON_Y 0x4000u
#define DK1_HOST_BUTTON_SELECT 0x2000u
#define DK1_HOST_BUTTON_START 0x1000u
#define DK1_HOST_BUTTON_UP 0x0800u
#define DK1_HOST_BUTTON_DOWN 0x0400u
#define DK1_HOST_BUTTON_LEFT 0x0200u
#define DK1_HOST_BUTTON_RIGHT 0x0100u
#define DK1_HOST_BUTTON_A 0x0080u
#define DK1_HOST_BUTTON_X 0x0040u
#define DK1_HOST_BUTTON_L 0x0020u
#define DK1_HOST_BUTTON_R 0x0010u
typedef struct Dk1HostInputState { uint16_t held; uint16_t pressed; } Dk1HostInputState;
uint16_t dk1_host_key_button(int key);
void dk1_host_input_update(Dk1HostInputState *state,uint16_t held);
#endif
