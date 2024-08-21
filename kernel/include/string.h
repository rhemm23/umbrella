#ifndef STRING_H
#define STRING_H

#include "type.h"

void memcpy(void *dest, void *src, uint32_t size);
void memset(void *data, uint8_t value, uint32_t size);
uint8_t memcmp(uint8_t *a, uint8_t *b, uint32_t size);

#endif
