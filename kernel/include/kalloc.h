#ifndef KALLOC_H
#define KALLOC_H

#define KALLOC_MIN_SIZE 0x10
#define KALLOC_START 0x00200000
#define KALLOC_END 0xF0000000

#include "type.h"

typedef struct {
  uint32_t status;
  uint32_t size;
} mem_header_t;

void * kalloc(uint32_t size);
void kfree(void *address);

void kalloc_init();
void kalloc_dump();

#endif
