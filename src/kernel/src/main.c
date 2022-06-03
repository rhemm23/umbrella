#include "serial.h"
#include "kalloc.h"
#include "type.h"
#include "vesa.h"
#include "log.h"
#include "pmm.h"
#include "vmm.h"
#include "exp.h"
#include "irq.h"
#include "idt.h"

void mem_init() {
  pmm_init();
  vmm_init();
  kalloc_init();
}

void int_init() {
  exp_init();
  irq_init();
  idt_init();
}

void main() {

  serial_init();
  log_init();

  mem_init();
  //int_init();

  uint8_t temp = 8 / 0;

  __asm__ __volatile__("cli; hlt");
}
