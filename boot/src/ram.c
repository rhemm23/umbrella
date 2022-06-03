#include "ram.h"

int ram_read_regions(uint16_t result_buffer) {

  uint16_t region_count_addr = result_buffer;
  result_buffer += 4;

  uint32_t list_index = 0;
  uint32_t carry = 0;
  uint32_t cnt = 0;

  while (1) {
    __asm__ __volatile__(
      "int $0x15"
      : :
      "a" (0xE820),
      "b" (list_index),
      "c" (RAM_REGION_DESC_SIZE),
      "d" (RAM_MAGIC_NUMBER),
      "D" (result_buffer)
    );
    __asm__ __volatile__(
      "mov $0, %%eax  \n\t"
      "adc $0, %%eax  \n\t"
      :
      "=a" (carry),
      "=b" (list_index)
    );
    if (carry || !list_index) {
      if (!cnt) {
        return 1;
      } else {
        *((uint32_t*)region_count_addr) = cnt;
        return 0;
      }
    } else {
      result_buffer += RAM_REGION_DESC_SIZE;
      cnt++;
    }
  }
}


