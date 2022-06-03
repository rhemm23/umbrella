#include "irq.h"
#include "pic.h"
#include "idt.h"

isr_t interrupt_handlers[256];

void irq_init() {

  pic_remap(0x20, 0x28);
  
  idt_set_entry(32, (uint32_t)irq0);
  idt_set_entry(33, (uint32_t)irq1);
  idt_set_entry(34, (uint32_t)irq2);
  idt_set_entry(35, (uint32_t)irq3);
  idt_set_entry(36, (uint32_t)irq4);
  idt_set_entry(37, (uint32_t)irq5);
  idt_set_entry(38, (uint32_t)irq6);
  idt_set_entry(39, (uint32_t)irq7);
  idt_set_entry(40, (uint32_t)irq8);
  idt_set_entry(41, (uint32_t)irq9);
  idt_set_entry(42, (uint32_t)irq10);
  idt_set_entry(43, (uint32_t)irq11);
  idt_set_entry(44, (uint32_t)irq12);
  idt_set_entry(45, (uint32_t)irq13);
  idt_set_entry(46, (uint32_t)irq14);
  idt_set_entry(47, (uint32_t)irq15);
}

void irq_register_handler(int index, isr_t handler) {
  if (index < 256) {
    interrupt_handlers[index] = handler;
  }
}

void irq_handler(register_t *reg) {
  if (interrupt_handlers[reg->int_no] != 0) {
    interrupt_handlers[reg->int_no](reg);
  }
  pic_ack(reg->int_no);
}