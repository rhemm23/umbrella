#ifndef DHCP_H
#define DHCP_H

#define DHCP_MAX_OPTIONS 4

#define DHCP_MAGIC_COOKIE 0x63825363

#define DHCP_OPT_SUBNET_MASK 0x01
#define DHCP_OPT_DOMAIN_SERV 0x06
#define DHCP_OPT_BROADCAST_ADDR 0x1C
#define DHCP_OPT_ADDR_REQ 0x32
#define DHCP_OPT_ADDR_DUR 0x33
#define DHCP_OPT_MSG_TYPE 0x35
#define DHCP_OPT_SERVER_IP 0x36

#define DHCP_MSG_DISCOVER 0x01
#define DHCP_MSG_OFFER 0x02
#define DHCP_MSG_REQUEST 0x03
#define DHCP_MSG_DECLINE 0x04
#define DHCP_MSG_ACK 0x05
#define DHCP_MSG_NAK 0x06
#define DHCP_MSG_RELEASE 0x07
#define DHCP_MSG_INFORM 0x08
#define DHCP_MSG_FORCE_RENEW 0x09
#define DHCP_MSG_LEASE_QUERY 0x0A
#define DHCP_MSG_LEASE_UNASSIGNED 0x0B
#define DHCP_MSG_LEASE_UNKNOWN 0x0C
#define DHCP_MSG_LEASE_ACTIVE 0x0D
#define DHCP_MSG_BULK_LEASE_QUERY 0x0E
#define DHCP_MSG_LEASE_QUERY_DONE 0x0F
#define DHCP_MSG_ACTIVE_LEASE_QUERY 0x10
#define DHCP_MSG_LEASE_QUERY_STATUS 0x11
#define DHCP_MSG_TLS 0x12

#define DHCP_ENDMARK 0xFF

#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_PORT 67

#define DHCP_OP_CLIENT 0x01
#define DHCP_OP_SERVER 0x02

#include "type.h"

typedef struct dhcp_option {
  uint8_t type;
  uint8_t len;
  uint8_t data[16];
} dhcp_option_t;

typedef struct dhcp_header {
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t tid;
  uint16_t secs;
  uint16_t flags;
  uint8_t ciaddr[4];
  uint8_t yiaddr[4];
  uint8_t siaddr[4];
  uint8_t giaddr[4];
  uint8_t chaddr[16];
  uint8_t sname[64];
  uint8_t bname[128];
} __attribute__((packed)) dhcp_header_t;

typedef struct dhcp_packet {
  dhcp_header_t header;
  dhcp_option_t options[DHCP_MAX_OPTIONS];
  int options_len;
} dhcp_packet_t;

void dhcp_handle_packet(uint8_t *packet, uint16_t packet_len);
void dhcp_send_packet(dhcp_packet_t *packet, uint8_t *dest_ip);
void dhcp_send_discover_message();

#endif
