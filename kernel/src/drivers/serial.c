#include "serial.h"
#include "port.h"

void serial_init() {
  port_write_byte(SERIAL_COM1 + 1, 0x00);
  port_write_byte(SERIAL_COM1 + 3, 0x80);
  port_write_byte(SERIAL_COM1 + 0, 0x03);
  port_write_byte(SERIAL_COM1 + 1, 0x00);
  port_write_byte(SERIAL_COM1 + 3, 0x03);
  port_write_byte(SERIAL_COM1 + 2, 0xC7);
  port_write_byte(SERIAL_COM1 + 4, 0x0B);
  port_write_byte(SERIAL_COM1 + 4, 0x1E);
  port_write_byte(SERIAL_COM1 + 4, 0x0F);
}

char serial_read_char() {
  while ((port_read_byte(SERIAL_COM1 + 5) & 0x01) == 0);
  return port_read_byte(SERIAL_COM1);
}

void serial_write_char(char value) {
  while ((port_read_byte(SERIAL_COM1 + 5) & 0x20) == 0);
  port_write_byte(SERIAL_COM1, value);
}

void serial_write_string(char *str) {
  while (*str) {
    serial_write_char(*str);
    str++;
  }
}
