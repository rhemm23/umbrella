#include "gdt.h"
#include "print.h"

static void gdt_set_entry(
  gdt_entry_t *entry,
  uint32_t base,
  uint32_t limit,
  uint8_t access,
  uint8_t flags
) {

  entry->base_low = (uint16_t)(base & 0xFFFF);
  entry->base_middle = (uint8_t)((base >> 16) & 0xFF);
  entry->base_high = (uint8_t)((base >> 24) & 0xFF);

  entry->limit_low = (uint16_t)(limit & 0xFFFF);
  entry->access = access;
  entry->granularity = ((flags << 4) & 0xF0) | (uint8_t)((limit >> 16) & 0x0F);
}

void gdt_configure() {

  gdt_ptr_t gdt_ptr = {
    .base = 0x0500,
    .size = 23
  };

  gdt_entry_t *gdt = (gdt_entry_t*)0x0500;
  gdt_set_entry(&gdt[0], 0, 0, 0, 0);
  gdt_set_entry(&gdt[1], 0, 0xFFFFF, 0x9A, 0xC);
  gdt_set_entry(&gdt[2], 0, 0xFFFFF, 0x92, 0xC);
}
