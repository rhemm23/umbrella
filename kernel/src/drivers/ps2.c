#include "port.h"
#include "ps2.h"

int ps2_data_available() {
  return (port_read_byte(PS2_CMD) & 1) == 1;
}

int ps2_send_available() {
  return (port_read_byte(PS2_CMD) & 2) == 0;
}

void ps2_write_command_register(uint8_t value) {
  while (!ps2_send_available());
  port_write_byte(PS2_CMD, value);
}

void ps2_write_data_register(uint8_t value) {
  while (!ps2_send_available());
  port_write_byte(PS2_DATA, value);
}

uint8_t ps2_read_data_register() {
  while (!ps2_data_available());
  return port_read_byte(PS2_DATA);
}

uint8_t ps2_read_controller_config_byte() {
  ps2_write_command_register(0x20);
  return ps2_read_data_register();
}

void ps2_write_controller_config_byte(uint8_t value) {
  ps2_write_command_register(0x60);
  ps2_write_data_register(value);
}

void ps2_enable_second_port() {
  ps2_write_command_register(0xA8);
}

void ps2_disable_second_port() {
  ps2_write_command_register(0xA7);
}

void ps2_write_second_port(uint8_t value) {
  ps2_write_command_register(0xD4);
  ps2_write_data_register(value);
}
