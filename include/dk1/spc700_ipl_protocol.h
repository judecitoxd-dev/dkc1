#ifndef DK1_SPC700_IPL_PROTOCOL_H
#define DK1_SPC700_IPL_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_SPC_IPL_READY0 0xAAu
#define DK1_SPC_IPL_READY1 0xBBu
#define DK1_SPC_IPL_BEGIN  0xCCu

typedef enum Dk1Spc700IplState {
    DK1_SPC_IPL_WAIT_BEGIN = 0,
    DK1_SPC_IPL_TRANSFER = 1,
    DK1_SPC_IPL_RUNNING = 2
} Dk1Spc700IplState;

typedef struct Dk1Spc700IplProtocol {
    uint8_t *ram;
    size_t ram_size;
    uint8_t ports[4];
    uint16_t write_address;
    uint16_t pc;
    uint8_t expected_token;
    uint64_t bytes_written;
    Dk1Spc700IplState state;
} Dk1Spc700IplProtocol;

bool dk1_spc700_ipl_init(Dk1Spc700IplProtocol *protocol,
                         uint8_t *ram, size_t ram_size);
bool dk1_spc700_ipl_begin(Dk1Spc700IplProtocol *protocol,
                          uint8_t command, uint16_t destination);
bool dk1_spc700_ipl_write_byte(Dk1Spc700IplProtocol *protocol,
                               uint8_t token, uint8_t value);
bool dk1_spc700_ipl_launch(Dk1Spc700IplProtocol *protocol,
                           uint8_t token, uint16_t entry_point);

#endif
