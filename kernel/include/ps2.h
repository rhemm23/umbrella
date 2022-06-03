#ifndef PS2_H
#define PS2_H

#define PS2_CMD 0x64
#define PS2_DATA 0x60

#include "type.h"

int ps2_data_available();
int ps2_send_available();

void ps2_write_controller_config_byte(uint8_t value);
void ps2_write_command_register(uint8_t value);
void ps2_write_data_register(uint8_t value);
void ps2_write_second_port(uint8_t value);

uint8_t ps2_read_data_register();
uint8_t ps2_read_controller_config_byte();

void ps2_enable_second_port();
void ps2_disable_second_port();

#endif
