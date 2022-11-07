#ifndef ARP_H
#define ARP_H

#define ARP_HARDWARE_ETHERNET 0x01

#define ARP_ADDR_MAX_LEN 16
#define ARP_MAX_SUPPORTED_PROTOCOLS 2
#define ARP_MAX_ENTRIES_PER_PROTOCOL 8

#define ARP_OP_REQUEST 0x01
#define ARP_OP_REPLY 0x02
#define ARP_OP_REQUEST_REV 0x03
#define ARP_OP_REPLY_REV 0x04
#define ARP_OP_DRARP_REQUEST 0x05
#define ARP_OP_DRARP_REPLY 0x06
#define ARP_OP_DRARP_ERROR 0x07
#define ARP_OP_IN_REPLY 0x08
#define ARP_OP_IN_ERROR 0x09
#define ARP_OP_NAK 0x0A
#define ARP_OP_MARS_REQUEST 0x0B
#define ARP_OP_MARS_MULTI 0x0C
#define ARP_OP_MARS_MSERV 0x0D
#define ARP_OP_MARS_JOIN 0x0E
#define ARP_OP_MARS_LEAVE 0x0F
#define ARP_OP_MARS_NAK 0x10
#define ARP_OP_MARS_UNSERV 0x11
#define ARP_OP_MARS_SJOIN 0x12
#define ARP_OP_MARS_SLEAVE 0x13
#define ARP_OP_MARS_GL_REQUEST 0x14
#define ARP_OP_MARS_GL_REPLY 0x15
#define ARP_OP_MARS_REDIRECT_MAP 0x16
#define ARP_OP_MAPOS_UNARP 0x17
#define ARP_OP_EXP1 0x18
#define ARP_OP_EXP2 0x19

#include "type.h"

typedef struct arp_entry {
  uint8_t protocol_addr[ARP_ADDR_MAX_LEN];
  uint8_t hardware_addr[ARP_ADDR_MAX_LEN];
  uint16_t hardware_type;
  uint8_t hardware_addr_len;
} arp_entry_t;

typedef struct arp_protocol_entry_set {
  int num_entries;
  uint16_t protocol_type;
  uint16_t protocol_addr_len;
  arp_entry_t entries[ARP_MAX_ENTRIES_PER_PROTOCOL];
} arp_protocol_entry_set_t;

typedef struct {
  uint16_t hardware_type;
  uint16_t protocol_type;
  uint8_t hardware_addr_len;
  uint8_t protocol_addr_len;
  uint16_t operation;
} __attribute__((packed)) arp_header_t;

int arp_lookup(
  uint16_t protocol_type,
  uint8_t *protocol_addr,
  uint8_t protocol_addr_len,
  uint8_t *hardware_addr,
  uint8_t *hardware_addr_len
);

void arp_add_ip4_addr(
  uint8_t *hardware_addr,
  uint8_t *ip_addr
);

void arp_init();
void arp_handle_packet(uint8_t *packet, uint16_t packet_len);
void arp_send_packet();

#endif
