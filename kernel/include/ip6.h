#ifndef IP6_H
#define IP6_H

#include "type.h"

typedef struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t traffic_class_low : 4;
  uint8_t version : 4;
  uint8_t flow_label_low : 4;
  uint8_t traffic_class_high : 4;
#else
  uint8_t version : 4;
  uint8_t traffic_class_low : 4;
  uint8_t traffic_class_high : 4;
  uint8_t flow_label_low : 4;
#endif
  uint16_t flow_label_high;
  uint16_t payload_length;
  uint8_t next_header;
  uint8_t hop_limit;
  uint8_t src_ip_addr[16];
  uint8_t dst_ip_addr[16];
} __attribute__ ((packed)) ip6_header_t;

void ip6_handle_packet(uint8_t *packet, uint16_t packet_len);

#endif
