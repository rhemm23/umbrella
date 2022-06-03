#ifndef ARP_H
#define ARP_H

#define ARP_HARDWARE_ETHERNET 0x01

#define ARP_MAX_ENTRIES 256

#define ARP_REQUEST 0x01
#define ARP_REPLY 0x02

#include "type.h"

typedef struct {
  uint8_t ip_addr[4];
  uint8_t mac_addr[6];
} arp_entry_t;

typedef struct {
  uint16_t hardware_type;
  uint16_t protocol_type;
  uint8_t hardware_addr_len;
  uint8_t protocol_addr_len;
  uint16_t operation;
} arp_header_t;

uint8_t * arp_lookup(uint8_t ip[4]);
void arp_init();

void arp_handle_packet(uint8_t *packet, uint16_t packet_len);
void arp_send_packet();

#endif
