#include "vesa.h"

vbe_controller_info_t vbe_controller_info;
vbe_mode_info_t vbe_mode_info;

void vesa_init() {

  uint32_t regions = *((uint32_t*)KERNEL_TRANSFER_MEM);
  uint32_t vbe_controller_info_addr = KERNEL_TRANSFER_MEM + sizeof(uint32_t) + regions * 24;

  vbe_controller_info = *((vbe_controller_info_t*)vbe_controller_info_addr);
  vbe_mode_info = *((vbe_mode_info_t*)(vbe_controller_info_addr + sizeof(vbe_controller_info_t)));
}
