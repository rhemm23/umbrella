#include "mouse.h"
#include "ps2.h"
#include "irq.h"

uint8_t mouse_packet[3];
uint8_t mouse_cycle = 0;

void mouse_interrupt_handler(register_t *reg) {
  mouse_packet[mouse_cycle] = ps2_read_data_register();
  mouse_cycle = (mouse_cycle + 1) % 3;
}

void mouse_write_command(uint8_t command) {
  ps2_write_second_port(command);
  ps2_read_data_register();
}

void mouse_init() {

  // Enable
  ps2_enable_second_port();

  // Enable interrupt
  uint8_t status = ps2_read_controller_config_byte();
  ps2_write_controller_config_byte(status | 2);

  // Setup mouse
  mouse_write_command(MOUSE_CMD_SET_DEFAULTS);
  mouse_write_command(MOUSE_CMD_ENABLE_STREAM);

  // Register handler
  irq_register_handler(44, mouse_interrupt_handler);
}
