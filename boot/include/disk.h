#ifndef DISK_H
#define DISK_H

#include "type.h"

typedef struct {
  uint8_t size;
  uint8_t zero;
  uint16_t sector_count;
  uint16_t buffer_ptr[2];
  uint64_t starting_sector;
} disk_packet_t;

int disk_read_sectors(uint16_t result_buffer, uint16_t sector_count, uint64_t starting_sector);

#endif
