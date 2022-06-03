#ifndef IP4_H
#define IP4_H

#define IP_DEFAULT_TTL 0x40

#define IP_PROTO_TCP 0x06
#define IP_PROTO_UDP 0x11

#include "type.h"

typedef struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t ihl : 4;
  uint8_t version : 4;
  uint8_t ecn : 2;
  uint8_t dscp : 6;
#else
  uint8_t version : 4;
  uint8_t ihl : 4;
  uint8_t dscp : 6;
  uint8_t ecn : 2;
#endif
  uint16_t total_length;
  uint16_t identification;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t fragment_offset_high : 5;
  uint8_t flags : 3;
#else
  uint8_t flags : 3;
  uint8_t fragment_offset_high : 5;
#endif
  uint8_t fragment_offset_low;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t header_checksum;
  uint8_t src_ip_addr[4];
  uint8_t dst_ip_addr[4];
} __attribute__((packed)) ip4_header_t;

extern uint8_t ip_address[4];

void ip4_handle_packet(uint8_t *packet, uint16_t packet_len);
void ip4_send_packet(uint8_t *packet, uint16_t packet_len, uint8_t dst_ip_addr[4], uint8_t protocol);

#endif
