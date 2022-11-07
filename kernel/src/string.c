#include "string.h"

void memset(void *data, uint8_t value, uint32_t size) {
  uint8_t *data_ptr = (uint8_t*)data;
  for (uint64_t i = 0; i < size; i++) {
    data_ptr[i] = value;
  }
}

void memcpy(void *dest, void *src, uint32_t size) {
  uint8_t *dest_ptr = (uint8_t*)dest;
  uint8_t *src_ptr = (uint8_t*)src;
  for (uint64_t i = 0; i < size; i++) {
    dest_ptr[i] = src_ptr[i];
  }
}
