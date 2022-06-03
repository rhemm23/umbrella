#include "kalloc.h"
#include "pmm.h"
#include "vmm.h"

void kalloc_init() {
  vmm_alloc_page(KALLOC_START);
  mem_header_t *header = (mem_header_t*)KALLOC_START;
  header->status = 0;
  header->size = KALLOC_END - KALLOC_START - sizeof(mem_header_t);
}

void * kalloc(uint32_t size) {
  if (size == 0) {
    return 0;
  } else if (size < KALLOC_MIN_SIZE) {
    size = KALLOC_MIN_SIZE;
  }
  uint32_t addr = KALLOC_START;
  uint32_t allocated_size = 0;
  while (addr < KALLOC_END) {
    mem_header_t *header = (mem_header_t*)addr;
    if (header->status == 0 && header->size >= size) {

      // Determine if we can split block
      int remaining_bytes = header->size - size;
      if (remaining_bytes >= sizeof(mem_header_t) + KALLOC_MIN_SIZE) {

        // Shrink block size
        header->size = size;

        // Assure we can access next block header
        uint32_t next_block = addr + sizeof(mem_header_t) + size;
        vmm_alloc_page(next_block);

        // Setup next block
        mem_header_t *split_header = (mem_header_t*)next_block;
        split_header->status = 0;
        split_header->size = remaining_bytes - sizeof(mem_header_t);
      }
      header->status = 1;
      addr += sizeof(mem_header_t);
      allocated_size = header->size;
      break;
    } else {
      addr += header->size + sizeof(mem_header_t);
    }
  }
  if (allocated_size == 0) {
    return 0;
  } else {
    uint32_t aligned_addr = addr;
    if (aligned_addr % MEM_PAGE_SIZE != 0) {
      aligned_addr -= aligned_addr % MEM_PAGE_SIZE;
    }
    for (uint32_t vmm_addr = aligned_addr; vmm_addr <= (addr + allocated_size); vmm_addr += MEM_PAGE_SIZE) {
      vmm_alloc_page(vmm_addr);
    }
    return (void*)addr;
  }
}

void kfree(void *address) {
  uint32_t value = (uint32_t)address;
  uint32_t cur_addr = KALLOC_START;
  uint32_t prev_addr = 0;
  while (cur_addr < KALLOC_END) {
    mem_header_t *header = (mem_header_t*)cur_addr;
    if (header->status == 1 && value == cur_addr + sizeof(mem_header_t)) {
      uint32_t next_addr = cur_addr + header->size + sizeof(mem_header_t);
      uint32_t lower_bound = cur_addr;
      uint32_t upper_bound = next_addr;
      header->status = 0;
      if (next_addr < KALLOC_END) {
        mem_header_t *next_header = (mem_header_t*)next_addr;
        if (next_header->status == 0) {
          header->size += sizeof(mem_header_t) + next_header->size;
          upper_bound += sizeof(mem_header_t);
        }
      }
      if (prev_addr > 0) {
        mem_header_t *prev_header = (mem_header_t*)prev_addr;
        if (prev_header->status == 0) {
          prev_header->size += sizeof(mem_header_t) + header->size;
          lower_bound -= sizeof(mem_header_t);
        }
      }
      if (lower_bound % MEM_PAGE_SIZE != 0) {
        lower_bound += MEM_PAGE_SIZE - (lower_bound % MEM_PAGE_SIZE);
      }
      if (upper_bound % MEM_PAGE_SIZE != 0) {
        upper_bound += MEM_PAGE_SIZE - (upper_bound % MEM_PAGE_SIZE);
      }
      upper_bound -= MEM_PAGE_SIZE;
      for (uint32_t page_bound = lower_bound; page_bound < upper_bound; page_bound += MEM_PAGE_SIZE) {
        vmm_free_page(page_bound);
      }
      return;
    }
    prev_addr = cur_addr;
    cur_addr += sizeof(mem_header_t) + header->size;
  }
}
