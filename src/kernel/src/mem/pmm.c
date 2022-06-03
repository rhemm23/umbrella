#include "string.h"
#include "pmm.h"

pmm_region_t mem_regions[MEM_MAX_REGIONS];
ram_region_t ram_regions[MEM_MAX_REGIONS];

uint32_t num_ram_regions;
uint32_t num_mem_regions;

uint32_t page_align(uint32_t value) {
  if (value % MEM_PAGE_SIZE != 0) {
    value += MEM_PAGE_SIZE - (value % MEM_PAGE_SIZE);
  }
  return value;
}

void read_ram_regions() {
  ram_region_t *ram_region_ptr = (ram_region_t*)(MEM_ADDRESS + sizeof(uint32_t));
  num_ram_regions = *((uint32_t*)MEM_ADDRESS);
  for (int i = 0; i < num_ram_regions; i++) {
    ram_regions[i] = ram_region_ptr[i];
  }
}

void calculate_useable_mem_regions() {
  num_mem_regions = 0;
  for (int i = 0; i < num_ram_regions; i++) {
    if (ram_regions[i].type == MEM_REGION_TYPE_NORMAL && ram_regions[i].size > 0) {
      uint32_t upper_bound_aligned = ram_regions[i].base_addr + ram_regions[i].size;
      uint32_t lower_bound_aligned = page_align(ram_regions[i].base_addr);
      if (upper_bound_aligned % MEM_PAGE_SIZE != 0) {
        upper_bound_aligned -= upper_bound_aligned % MEM_PAGE_SIZE;
      }
      if (upper_bound_aligned <= MEM_KERNEL_END) {
        continue;
      } else if (lower_bound_aligned < MEM_KERNEL_END) {
        lower_bound_aligned = MEM_KERNEL_END;
      }
      uint32_t size_in_pages = (upper_bound_aligned - lower_bound_aligned) / MEM_PAGE_SIZE;
      uint32_t bits_per_page = MEM_PAGE_SIZE * 8;
      if (size_in_pages > 1) {
        uint32_t bitmap_pages = (size_in_pages + bits_per_page - 1) / bits_per_page;
        mem_regions[num_mem_regions].base_addr = lower_bound_aligned;
        mem_regions[num_mem_regions].high_addr = upper_bound_aligned;
        mem_regions[num_mem_regions].bitmap_pages = bitmap_pages;
        mem_regions[num_mem_regions].data_pages = size_in_pages - bitmap_pages;
        num_mem_regions++;
        memset((void*)lower_bound_aligned, 0x00, bitmap_pages * MEM_PAGE_SIZE);
      }
    }
  }
}

void pmm_init() {
  read_ram_regions();
  calculate_useable_mem_regions();
}

void pmm_free(uint32_t block) {
  if (block % MEM_PAGE_SIZE != 0) {
    return;
  }
  for (int i = 0; i < num_mem_regions; i++) {
    if (mem_regions[i].high_addr > block && block > mem_regions[i].base_addr) {
      uint32_t page_offset = (block - mem_regions[i].base_addr) / MEM_PAGE_SIZE;
      if (page_offset < mem_regions[i].bitmap_pages) {
        return;
      }
      page_offset -= mem_regions[i].bitmap_pages;
      uint8_t *base = (uint8_t*)mem_regions[i].base_addr;
      base[page_offset / 8] &= ~(0x01 << (page_offset % 8));
      return;
    }
  }
}

uint32_t pmm_allocate() {
  for (int i = 0; i < num_mem_regions; i++) {
    for (int j = 0; j < (mem_regions[i].bitmap_pages * MEM_PAGE_SIZE); j++) {
      uint8_t *base = (uint8_t*)mem_regions[i].base_addr;
      if (base[j] != 0xFF) {
        for (int k = 0; k < 8; k++) {
          if (((base[j] >> k) & 0x01) == 0) {
            uint32_t skip_pages = mem_regions[i].bitmap_pages + (j * 8) + k;
            uint32_t address = mem_regions[i].base_addr + (skip_pages * MEM_PAGE_SIZE);
            base[j] |= (0x01 << k);
            return address;
          }
        }
      }
    }
  }
  return 0;
}
