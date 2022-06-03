#include "string.h"

void memset(void *data, uint8_t value, uint32_t size) {
  uint8_t *data_ptr = (uint8_t*)data;
  for (uint64_t i = 0; i < size; i++) {
    data_ptr[i] = value;
  }
}
