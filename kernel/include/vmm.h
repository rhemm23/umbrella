#ifndef VMM_H
#define VMM_H

#define PG_PRESENT_FLAG 0x01
#define PG_RW_FLAG 0x02
#define PG_USER_FLAG 0x03

#include "type.h"

typedef uint32_t pde_t;
typedef uint32_t pte_t;

typedef struct {
  pte_t pages[1024];
} page_table_t;

typedef struct {
  pde_t tables[1024];
} page_dir_t;

void vmm_init();
void vmm_free_page(uint32_t virtual_address);
void vmm_alloc_page(uint32_t virtual_address);
void vmm_map_page(uint32_t physical_address, uint32_t virtual_address);

uint32_t vmm_virtual_to_physical(uint32_t virtual_address);

#endif
