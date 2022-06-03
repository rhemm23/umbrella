#ifndef ETHERNET_H
#define ETHERNET_H

#define ETHERNET_TYPE_IP4 0x0800
#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_TYPE_IP6 0x86DD

#include "type.h"

typedef struct {
  uint8_t dst_mac_addr[6];
  uint8_t src_mac_addr[6];
  uint16_t type;
} ethernet_header_t;

void ethernet_handle_packet(uint8_t *packet, uint16_t packet_len);
void ethernet_send_packet(uint8_t *packet, uint16_t packet_len, uint8_t dst_mac[6], uint16_t type);

#endif
