#ifndef GDT_H
#define GDT_H

#include "type.h"

typedef struct {
  uint16_t size;
  uint32_t base;
} gdt_ptr_t;

typedef struct {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} gdt_entry_t;

void gdt_set_entry(
  gdt_entry_t *entry,
  uint32_t base,
  uint32_t limit,
  uint8_t access,
  uint8_t flags
);

#endif
