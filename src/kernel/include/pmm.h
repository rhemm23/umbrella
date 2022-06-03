#ifndef PMM_H
#define PMM_H

#define MEM_PAGE_SIZE 4096
#define MEM_KERNEL_END 0x00200000

#define MEM_MAX_REGIONS 64
#define MEM_ADDRESS 0x1000

#define MEM_REGION_TYPE_NORMAL 0x01
#define MEM_REGION_TYPE_RESERVED 0x02

#include "type.h"

typedef struct {
  uint64_t base_addr;
  uint64_t size;
  uint32_t type;
  uint32_t acpi;
} ram_region_t;

typedef struct {
  uint32_t base_addr;
  uint32_t high_addr;
  uint32_t bitmap_pages;
  uint32_t data_pages;
} pmm_region_t;

extern uint32_t num_mem_regions;
extern uint32_t num_ram_regions;

extern ram_region_t ram_regions[MEM_MAX_REGIONS];
extern pmm_region_t mem_regions[MEM_MAX_REGIONS];

void pmm_init();
void pmm_free(uint32_t block);
uint32_t pmm_allocate();

#endif
