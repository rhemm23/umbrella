#include "port.h"

void port_write_byte(uint16_t port, uint8_t value) {
  __asm__ __volatile__(
    "out %%al, %%dx"
    : :
    "a" (value),
    "d" (port)
  );
}

void port_write_word(uint16_t port, uint16_t value) {
  __asm__ __volatile__(
    "out %%ax, %%dx"
    : :
    "a" (value),
    "d" (port)
  );
}

void port_write_dword(uint16_t port, uint32_t value) {
  __asm__ __volatile__(
    "out %%eax, %%dx"
    : :
    "a" (value),
    "d" (port)
  );
}

uint8_t port_read_byte(uint16_t port) {
  uint8_t value;
  __asm__ __volatile__(
    "in %%dx, %%al"
    :
    "=a" (value)
    :
    "d" (port)
  );
  return value;
}

uint16_t port_read_word(uint16_t port) {
  uint16_t value;
  __asm__ __volatile__(
    "in %%dx, %%ax"
    :
    "=a" (value)
    :
    "d" (port)
  );
  return value;
}

uint32_t port_read_dword(uint16_t port) {
  uint32_t value;
  __asm__ __volatile__(
    "in %%dx, %%eax"
    :
    "=a" (value)
    :
    "d" (port)
  );
  return value;
}
