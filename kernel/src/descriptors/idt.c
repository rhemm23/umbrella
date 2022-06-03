#include "idt.h"

extern void idt_flush(uint32_t addr);

idt_entry_t idt_entries[IDT_MAX_ENTRIES];
idt_ptr_t idt_ptr;

void idt_init() {

  idt_ptr.base = (uint32_t)&idt_entries;
  idt_ptr.limit = sizeof(idt_entries) - 1;

  idt_flush((uint32_t)&idt_ptr);

  __asm__ __volatile__("sti");
}

void idt_set_entry(int n, uint32_t handler) {
  idt_entries[n].offset_low = (handler & 0xFFFF);
  idt_entries[n].segment_select = KERNEL_CODE_SEGMENT;
  idt_entries[n].reserved = 0;
  idt_entries[n].flags = 0x80 | IDT_INTERRUPT_GATE_32;
  idt_entries[n].offset_high = ((handler >> 16) & 0xFFFF);
}
