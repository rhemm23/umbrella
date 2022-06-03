#ifndef DHCP_H
#define DHCP_H

#define DHCP_MAGIC_COOKIE 0x63825363

#define DHCP_OPT_MSG_TYPE 0x35

#define DHCP_MSG_DISCOVER 0x01
#define DHCP_MSG_OFFER 0x02
#define DHCP_MSG_REQUEST 0x03
#define DHCP_MSG_DECLINE 0x04

#define DHCP_ENDMARK 0xFF

#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_PORT 67

#define DHCP_OP_CLIENT 0x01
#define DHCP_OP_SERVER 0x02

#include "type.h"

typedef struct {
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t tid;
  uint16_t secs;
  uint16_t flags;
  uint32_t ciaddr;
  uint32_t yiaddr;
  uint32_t siaddr;
  uint32_t giaddr;
  uint8_t chaddr[16];
  uint8_t sname[64];
  uint8_t bname[128];
} dhcp_header_t;

void dhcp_handle_packet(uint8_t *packet, uint16_t packet_len);
void dhcp_send_discover_message();

#endif
