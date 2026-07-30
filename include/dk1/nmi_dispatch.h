#ifndef DK1_NMI_DISPATCH_H
#define DK1_NMI_DISPATCH_H

#include <stdbool.h>
#include <stdint.h>

typedef enum Dk1NmiCallbackKind {
    DK1_NMI_CALLBACK_UNKNOWN = 0,
    DK1_NMI_CALLBACK_RESTORE_DISPLAY_AND_RETURN,
    DK1_NMI_CALLBACK_RETURN_ONLY,
    DK1_NMI_CALLBACK_MAIN_FRAME,
} Dk1NmiCallbackKind;

typedef struct Dk1NmiDispatchPlan {
    uint16_t callback_address;
    uint8_t callback_bank;
    Dk1NmiCallbackKind kind;
    bool acknowledge_nmi;
    bool force_blank_on_entry;
    bool restore_display_before_rti;
    bool resets_stack_for_frame;
    bool returns_with_rti;
} Dk1NmiDispatchPlan;

Dk1NmiDispatchPlan dk1_nmi_dispatch_plan(uint16_t callback_address);

#endif
