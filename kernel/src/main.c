#include "rtl8139.h"
#include "serial.h"
#include "kalloc.h"
#include "mouse.h"
#include "type.h"
#include "vesa.h"
#include "dhcp.h"
#include "log.h"
#include "pmm.h"
#include "vmm.h"
#include "exp.h"
#include "irq.h"
#include "idt.h"
#include "pci.h"
#include "arp.h"

void net_init() {
  rtl8139_init();
  arp_init();
}

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
  int_init();
  pci_init();
  net_init();

  dhcp_send_discover_message();

  while (1);
}
