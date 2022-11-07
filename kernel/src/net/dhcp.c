#include "rtl8139.h"
#include "string.h"
#include "dhcp.h"
#include "dns.h"
#include "net.h"
#include "udp.h"
#include "log.h"
#include "ip4.h"
#include "arp.h"

uint8_t domain_server_ip[4];

typedef enum {
  DHCP_STATE_INIT,
  DHCP_STATE_SENT_DISCOVER,
  DHCP_STATE_RECEIVED_OFFER,
  DHCP_STATE_SENT_REQUEST,
  DHCP_STATE_RECEIVED_ACK
} dhcp_state_t;

typedef struct dhcp_options {
  uint8_t message_type;
  uint8_t server_ip[4];
  uint8_t subnet_mask[4];
  uint8_t broadcast_ip[4];
  uint8_t domain_server_ip[4];
  uint32_t addr_lease_time;
} dhcp_options_t;

static dhcp_state_t state = DHCP_STATE_INIT;

static uint32_t uint_from_bytes(uint8_t *bytes) {
  uint32_t value = 0;
  for (int i = 0; i < 4; i++) {
    value |= ((uint32_t)bytes[i] << (24 - i * 8));
  }
  return value;
}

static void parse_dhcp_options(dhcp_options_t *options, uint8_t *data, int data_len) {
  int index = 0;
  while (data_len > 0) {
    uint8_t option_type = data[index++];
    if (option_type > 0) {
      uint8_t option_len = data[index++];
      switch (option_type) {
        case DHCP_OPT_MSG_TYPE:
          options->message_type = data[index];
          break;

        case DHCP_OPT_SERVER_IP:
          memcpy(options->server_ip, &data[index], 4);
          break;

        case DHCP_OPT_SUBNET_MASK:
          memcpy(options->subnet_mask, &data[index], 4);
          break;

        case DHCP_OPT_BROADCAST_ADDR:
          memcpy(options->broadcast_ip, &data[index], 4);
          break;

        case DHCP_OPT_ADDR_DUR:
          options->addr_lease_time = uint_from_bytes(&data[index]);
          break;

        case DHCP_OPT_DOMAIN_SERV:
          memcpy(options->domain_server_ip, &data[index], 4);
          break;
      }
      index += option_len;
      data_len -= 2 + option_len;
    } else {
      data_len--;
    }
  }
}

uint32_t dhcp_calculate_tid() {
  uint32_t result = 0;
  for (int i = 0; i < 4; i++) {
    result |= (uint32_t)mac_addr[i] << (i * 8);
  }
  return result;
}

void dhcp_send_packet(dhcp_packet_t *packet, uint8_t *dest_ip) {

  // Header and magic cookie
  uint32_t packet_len = sizeof(dhcp_header_t) + 5;
  for (int i = 0; i < packet->options_len; i++) {
    packet_len += packet->options[i].len + 2;
  }

  uint8_t packet_data[packet_len];

  // Copy header data
  uint8_t *header_data = (uint8_t*)&packet->header;
  memcpy(packet_data, header_data, sizeof(dhcp_header_t));
  int index = sizeof(dhcp_header_t);

  // Copy magic cookie
  *((uint32_t*)&packet_data[index]) = htonl(DHCP_MAGIC_COOKIE);
  index += 4;

  // Copy options
  for (int i = 0; i < packet->options_len; i++) {
    packet_data[index++] = packet->options[i].type;
    packet_data[index++] = packet->options[i].len;
    memcpy(&packet_data[index], packet->options[i].data, packet->options[i].len);
    index += packet->options[i].len;
  }

  // Add endmark
  packet_data[index] = 0xFF;

  // Transmit
  udp_send_packet(
    &packet_data[0],
    packet_len,
    dest_ip,
    DHCP_CLIENT_PORT,
    DHCP_SERVER_PORT
  );
}

void dhcp_send_request_message(uint8_t *requested_ip, uint8_t *server_ip) {

  dhcp_packet_t packet;
  memset(&packet.header, 0x00, sizeof(dhcp_header_t));

  packet.header.op = DHCP_OP_CLIENT;
  packet.header.htype = 0x01;
  packet.header.hlen = 0x06;
  packet.header.tid = htonl(dhcp_calculate_tid());

  memcpy(packet.header.ciaddr, requested_ip, 4);
  memcpy(packet.header.siaddr, server_ip, 4);
  memcpy(packet.header.chaddr, mac_addr, 6);

  packet.options_len = 3;
  packet.options[0].type = DHCP_OPT_MSG_TYPE;
  packet.options[0].len = 1;
  packet.options[0].data[0] = DHCP_MSG_REQUEST;

  packet.options[1].type = DHCP_OPT_ADDR_REQ;
  packet.options[1].len = 4;
  memcpy(packet.options[1].data, requested_ip, 4);

  packet.options[2].type = DHCP_OPT_SERVER_IP;
  packet.options[2].len = 4;
  memcpy(packet.options[2].data, server_ip, 4);

  uint8_t dest_ip[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
  dhcp_send_packet(&packet, dest_ip);

  state = DHCP_STATE_SENT_REQUEST;
}

void dhcp_send_discover_message() {

  dhcp_header_t header;
  uint8_t *packet_header = (uint8_t*)&header;

  // Zero out header
  memset(packet_header, 0x00, sizeof(dhcp_header_t));

  header.op = DHCP_OP_CLIENT;
  header.htype = 0x01;
  header.hlen = 0x06;
  header.tid = htonl(dhcp_calculate_tid());

  memcpy(header.chaddr, mac_addr, 6);

  dhcp_packet_t packet;
  packet.header = header;
  packet.options_len = 1;
  packet.options[0].type = DHCP_OPT_MSG_TYPE;
  packet.options[0].len = 1;
  packet.options[0].data[0] = DHCP_MSG_DISCOVER;

  uint8_t dest_ip[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
  dhcp_send_packet(&packet, dest_ip);

  // Update state
  state = DHCP_STATE_SENT_DISCOVER;
}

void dhcp_handle_packet(uint8_t *packet, uint16_t packet_len) {

  dhcp_header_t *header = (dhcp_header_t*)packet;
  uint32_t tid = ntohl(header->tid);

  uint32_t index = sizeof(dhcp_header_t) + 4;
  uint32_t options_len = packet_len - index;

  dhcp_options_t options;
  parse_dhcp_options(&options, &packet[index], options_len);

  if (tid != dhcp_calculate_tid()) {
    return;
  }

  switch (state) {
    case DHCP_STATE_SENT_DISCOVER:
      if (options.message_type == DHCP_MSG_OFFER) {
        dhcp_send_request_message(header->yiaddr, options.server_ip);
        log_info("(DHCP) Sending Request");
      }
      break;

    case DHCP_STATE_SENT_REQUEST:
      if (options.message_type == DHCP_MSG_ACK) {
        memcpy(ip_address, header->yiaddr, 4);
        memcpy(domain_server_ip, options.domain_server_ip, 4);
        log_info("(DHCP) Received Acknowledge");
        state = DHCP_STATE_RECEIVED_ACK;
      }
      break;
  }
}
