#ifndef ICMP6
#define ICMP6

#include "type.h"

typedef struct {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
} __attribute__ ((packed)) icmp6_header_t;

void icmp6_handle_packet(uint8_t *packet, uint16_t packet_len);

#endif
