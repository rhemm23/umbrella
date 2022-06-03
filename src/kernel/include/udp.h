#ifndef UDP_H
#define UDP_H

#include "type.h"

typedef struct {
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t length;
  uint16_t checksum;
} udp_header_t;

void udp_handle_packet(uint8_t *packet, uint16_t packet_len);
void udp_send_packet(uint8_t *packet, uint16_t packet_len, uint8_t dst_ip[4], uint16_t src_port, uint16_t dst_port);

#endif
