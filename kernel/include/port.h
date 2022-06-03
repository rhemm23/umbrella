#ifndef PORT_H
#define PORT_H

#include "type.h"

uint8_t port_read_byte(uint16_t port);
uint16_t port_read_word(uint16_t port);
uint32_t port_read_dword(uint16_t port);

void port_write_byte(uint16_t port, uint8_t value);
void port_write_word(uint16_t port, uint16_t value);
void port_write_dword(uint16_t port, uint32_t value);

#endif
