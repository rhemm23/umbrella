#include "disk.h"

int disk_read_sectors(uint16_t result_buffer, uint16_t sector_count, uint64_t starting_sector) {

  disk_packet_t dap = {
    .size = 0x10,
    .zero = 0x00,
    .sector_count = sector_count,
    .starting_sector = starting_sector
  };

  dap.buffer_ptr[0] = result_buffer;
  dap.buffer_ptr[1] = 0x00;

  uint8_t result;

  __asm__ __volatile__(
    "mov %1, %%si     \n\t"
    "mov $0x42, %%ah  \n\t"
    "mov $0x80, %%dl  \n\t"
    "int $0x13        \n\t"
    "mov %%ah, %0     \n\t"
    :
    "=r" (result)
    :
    "r" ((uint16_t)&dap)
    :
    "si",
    "ah",
    "dl"
  );

  return result != 0x00;
}
