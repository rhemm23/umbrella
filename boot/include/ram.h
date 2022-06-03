#ifndef RAM_H
#define RAM_H

#define RAM_MAGIC_NUMBER 0x534D4150
#define RAM_REGION_DESC_SIZE 24

#include "type.h"

int ram_read_regions(uint16_t result_buffer);

#endif
