#include "idt.h"

extern void idt_flush();

void idt_init() {
  idt_flush();
  __asm__ __volatile__("sti");
}

void idt_set_entry(int n, uint32_t handler) {
  idt_entry_t *idt_entries = (idt_entry_t*)0x5000;
  idt_entries[n].offset_low = (handler & 0xFFFF);
  idt_entries[n].segment_select = KERNEL_CODE_SEGMENT;
  idt_entries[n].reserved = 0;
  idt_entries[n].flags = 0x80 | IDT_INTERRUPT_GATE_32;
  idt_entries[n].offset_high = ((handler >> 16) & 0xFFFF);
}
