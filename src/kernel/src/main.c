#include "serial.h"
#include "kalloc.h"
#include "type.h"
#include "vesa.h"
#include "log.h"
#include "pmm.h"
#include "vmm.h"

void mem_init() {
  pmm_init();
  vmm_init();
  kalloc_init();
}

void main() {

  serial_init();
  log_init();

  mem_init();

  log_info("hello!");
  log_warn("uh oh!");
  log_error("danger!");

  __asm__ __volatile__("cli; hlt");
}
