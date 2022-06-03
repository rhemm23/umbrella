#include "exp.h"
#include "idt.h"
#include "log.h"

char *exception_messages[32] = {
  "Division By Zero",
  "Debug",
  "Non Maskable Interrupt",
  "Breakpoint",
  "Into Detected Overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No Coprocessor",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Bad TSS",
  "Segment Not Present",
  "Stack Fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown Interrupt",
  "Coprocessor Fault",
  "Alignment Check",
  "Machine Check",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved"
};

void exp_init() {
  idt_set_entry(0, (uint32_t)exception0);
  idt_set_entry(1, (uint32_t)exception1);
  idt_set_entry(2, (uint32_t)exception2);
  idt_set_entry(3, (uint32_t)exception3);
  idt_set_entry(4, (uint32_t)exception4);
  idt_set_entry(5, (uint32_t)exception5);
  idt_set_entry(6, (uint32_t)exception6);
  idt_set_entry(7, (uint32_t)exception7);
  idt_set_entry(8, (uint32_t)exception8);
  idt_set_entry(9, (uint32_t)exception9);
  idt_set_entry(10, (uint32_t)exception10);
  idt_set_entry(11, (uint32_t)exception11);
  idt_set_entry(12, (uint32_t)exception12);
  idt_set_entry(13, (uint32_t)exception13);
  idt_set_entry(14, (uint32_t)exception14);
  idt_set_entry(15, (uint32_t)exception15);
  idt_set_entry(16, (uint32_t)exception16);
  idt_set_entry(17, (uint32_t)exception17);
  idt_set_entry(18, (uint32_t)exception18);
  idt_set_entry(19, (uint32_t)exception19);
  idt_set_entry(20, (uint32_t)exception20);
  idt_set_entry(21, (uint32_t)exception21);
  idt_set_entry(22, (uint32_t)exception22);
  idt_set_entry(23, (uint32_t)exception23);
  idt_set_entry(24, (uint32_t)exception24);
  idt_set_entry(25, (uint32_t)exception25);
  idt_set_entry(26, (uint32_t)exception26);
  idt_set_entry(27, (uint32_t)exception27);
  idt_set_entry(28, (uint32_t)exception28);
  idt_set_entry(29, (uint32_t)exception29);
  idt_set_entry(30, (uint32_t)exception30);
  idt_set_entry(31, (uint32_t)exception31);
}

void exception_handler(register_t reg) {
  log_error(exception_messages[reg.int_no]);
}
