#include "print.h"
#include "disk.h"
#include "vesa.h"
#include "ram.h"
#include "gdt.h"

#define KERNEL_TRANSFER_MEM 0x1000

extern void exit();

static void die(char *err) {
  print(err);
  __asm__ __volatile__("cli; hlt");
}

void main() {

  if (disk_read_sectors(0x8E00, 32, 9)) {
    die("Failed to read kernel from disk");
  }

  if (ram_read_regions(KERNEL_TRANSFER_MEM)) {
    die("Failed to read ram regions");
  }

  uint32_t regions = *((uint32_t*)KERNEL_TRANSFER_MEM);
  uint16_t vesa_buffer = KERNEL_TRANSFER_MEM + sizeof(uint32_t) + regions * RAM_REGION_DESC_SIZE;

  if (vesa_configure(vesa_buffer)) {
    die("Failed to configure vesa mode");
  }

  gdt_configure();
  exit();
}
