#include "port.h"
#include "pic.h"

void pic_remap(uint8_t master_offset, uint8_t slave_offset) {

  // Send init command
  port_write_byte(PIC_MASTER_COMMAND, PIC_INIT);
  port_write_byte(PIC_SLAVE_COMMAND, PIC_INIT);

  // Send new offsets
  port_write_byte(PIC_MASTER_DATA, master_offset);
  port_write_byte(PIC_SLAVE_DATA, slave_offset);

  // Locate slave
  port_write_byte(PIC_MASTER_DATA, 0x04);
  port_write_byte(PIC_SLAVE_DATA, 0x02);

  port_write_byte(PIC_MASTER_DATA, PIC_8086);
  port_write_byte(PIC_SLAVE_DATA, PIC_8086);

  port_write_byte(PIC_MASTER_DATA, 0x00);
  port_write_byte(PIC_SLAVE_DATA, 0x00);
}