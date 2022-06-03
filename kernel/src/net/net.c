#include "net.h"

uint32_t reverse_uint32(uint32_t value) {
  uint32_t result = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t byte = (value >> (i * 8)) & 0xFF;
    result |= byte << ((3 - i) * 8);
  }
  return result;
}

uint16_t ntohs(uint16_t value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return value;
#else
  return (value << 8) | (value >> 8);
#endif
}

uint16_t htons(uint16_t value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return value;
#else
  return (value << 8) | (value >> 8);
#endif
}

uint32_t ntohl(uint32_t value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return value;
#else
  return reverse_uint32(value);
#endif
}

uint32_t htonl(uint32_t value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return value;
#else
  return reverse_uint32(value);
#endif
}
