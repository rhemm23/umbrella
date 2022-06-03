#ifndef IDT_H
#define IDT_H

#define IDT_MAX_ENTRIES 256
#define KERNEL_CODE_SEGMENT 0x08

#define IDT_TRAP_GATE_16 0x07
#define IDT_TRAP_GATE_32 0x0F

#define IDT_INTERRUPT_GATE_16 0x06
#define IDT_INTERRUPT_GATE_32 0x0E

#include "type.h"

typedef struct {
  uint16_t offset_low;
  uint16_t segment_select;
  uint8_t reserved;
  uint8_t flags;
  uint16_t offset_high;
} idt_entry_t;

typedef struct {
  uint16_t limit;
  uint32_t base;
} idt_ptr_t;

void idt_init();
void idt_set_entry(int n, uint32_t handler);

#endif
