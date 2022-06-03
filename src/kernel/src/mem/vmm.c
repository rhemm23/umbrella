#include "string.h"
#include "pmm.h"
#include "vmm.h"
#include "log.h"

page_dir_t *kernel_dir;

void vmm_set_dir(page_dir_t *dir) {
  uint32_t value = (uint32_t)dir;
  __asm__ __volatile__("mov %%eax, %%cr3" : : "a" (value));
}

void vmm_enable_paging() {
  __asm__ __volatile__("mov %cr0, %eax");
  __asm__ __volatile__("orl $0x80000000, %eax");
  __asm__ __volatile__("mov %eax, %cr0");
}

uint32_t vmm_page_dir_index(uint32_t virtual_address) {
  return (virtual_address >> 22) & 0x3FF;
}

uint32_t vmm_page_tab_index(uint32_t virtual_address) {
  return (virtual_address >> 12) & 0x3FF;
}

page_table_t * vmm_virtual_to_table(uint32_t virtual_address) {
  return (page_table_t*)(0xFFC00000 + vmm_page_dir_index(virtual_address) * MEM_PAGE_SIZE);
}

pte_t * vmm_virtual_to_entry(uint32_t virtual_address) {

  uint32_t dir_index = vmm_page_dir_index(virtual_address);
  uint32_t tab_index = vmm_page_tab_index(virtual_address);

  page_dir_t *dir = (page_dir_t*)0xFFFFF000;
  pde_t *pde = (pde_t*)&dir->tables[dir_index];

  if ((*pde & PG_PRESENT_FLAG) == 0) {
    return 0;
  }

  page_table_t *table = vmm_virtual_to_table(virtual_address);
  return (pte_t*)&table->pages[tab_index];
}

uint32_t vmm_virtual_to_physical(uint32_t virtual_address) {

  pte_t *pte = vmm_virtual_to_entry(virtual_address);

  if (!pte || !(*pte & PG_PRESENT_FLAG)) {
    return 0;
  }

  return (*pte & ~0xFFF) + (virtual_address & 0xFFF);
}

void vmm_map_page(uint32_t physical_address, uint32_t virtual_address) {

  uint32_t dir_index = vmm_page_dir_index(virtual_address);
  uint32_t tab_index = vmm_page_tab_index(virtual_address);

  page_dir_t *dir = (page_dir_t*)0xFFFFF000;
  pde_t *pde = (pde_t*)&dir->tables[dir_index];

  if ((*pde & PG_PRESENT_FLAG) == 0) {
    uint32_t page_table = pmm_allocate();
    *pde = page_table | PG_RW_FLAG | PG_PRESENT_FLAG;
    memset((void*)vmm_virtual_to_table(virtual_address), 0x00, sizeof(page_table_t));
  }

  page_table_t *table = vmm_virtual_to_table(virtual_address);
  pte_t *pte = (pte_t*)&table->pages[tab_index];

  *pte = physical_address | PG_RW_FLAG | PG_PRESENT_FLAG;
}

void vmm_alloc_page(uint32_t virtual_address) {

  pte_t *pte = vmm_virtual_to_entry(virtual_address);

  if (!pte || !(*pte & PG_PRESENT_FLAG)) {
    uint32_t page = pmm_allocate();
    vmm_map_page(page, virtual_address);
  }
}

void vmm_free_page(uint32_t virtual_address) {

  pte_t *pte = vmm_virtual_to_entry(virtual_address);

  if (!pte || !(*pte & PG_PRESENT_FLAG)) {
    return;
  }

  pmm_free(*pte & ~0xFFF);
  *pte &= ~PG_PRESENT_FLAG;

  // Flush page dir
  vmm_set_dir(kernel_dir);
}

void vmm_init() {

  // Allocate kernel page directory
  kernel_dir = (page_dir_t*)pmm_allocate();
  for (int i = 0; i < 1024; i++) {
    kernel_dir->tables[i] = PG_RW_FLAG;
  }

  // Identity map first 2 megabytes (where kernel lives)
  page_table_t *table = (page_table_t*)pmm_allocate();
  for (int i = 0; i < 612; i++) {
    table->pages[i] = (i * 0x1000) | PG_PRESENT_FLAG | PG_RW_FLAG;
  }
  for (int i = 612; i < 1024; i++) {
    table->pages[i] = PG_RW_FLAG;
  }

  // Add table to directory
  kernel_dir->tables[0] = (uint32_t)table | PG_PRESENT_FLAG;
  kernel_dir->tables[1023] = (uint32_t)kernel_dir | PG_PRESENT_FLAG;

  // Load into cr3
  vmm_set_dir(kernel_dir);
  vmm_enable_paging();
}
