
#include "rtl8139.h"
#include "dhcp.h"
#include "net.h"
#include "udp.h"
#include "log.h"

uint32_t dhcp_calculate_tid() {
  uint32_t result = 0;
  for (int i = 0; i < 4; i++) {
    result |= (uint32_t)mac_addr[i] << (i * 8);
  }
  return result;
}

void dhcp_handle_packet(uint8_t *packet, uint16_t packet_len) {

  dhcp_header_t *header = (dhcp_header_t*)packet;
  uint32_t tid = ntohl(header->tid);

  if (tid == dhcp_calculate_tid()) {
    log_info("(DHCP) Received Message");
  }
}

void dhcp_send_discover_message() {

  dhcp_header_t header;
  uint8_t *packet_header = (uint8_t*)&header;

  // Zero out header
  for (int i = 0; i < sizeof(dhcp_header_t); i++) {
    packet_header[i] = 0x00;
  }

  header.op = DHCP_OP_CLIENT;
  header.htype = 0x01;
  header.hlen = 0x06;
  header.tid = htonl(dhcp_calculate_tid());
  
  // Copy mac
  for (int i = 0; i < 6; i++) {
    header.chaddr[i] = mac_addr[i];
  }

  // Copy header to raw data
  uint8_t packet_data[sizeof(dhcp_header_t) + 8];
  for (int i = 0; i < sizeof(dhcp_header_t); i++) {
    packet_data[i] = packet_header[i];
  }

  // Add DHCP options
  *((uint32_t*)(packet_data + sizeof(dhcp_header_t))) = htonl(DHCP_MAGIC_COOKIE);
  packet_data[sizeof(dhcp_header_t) + 4] = DHCP_OPT_MSG_TYPE;
  packet_data[sizeof(dhcp_header_t) + 5] = 0x01;
  packet_data[sizeof(dhcp_header_t) + 6] = DHCP_MSG_DISCOVER;
  packet_data[sizeof(dhcp_header_t) + 7] = DHCP_ENDMARK;

  // Transmit
  udp_send_packet(
    &packet_data[0],
    sizeof(dhcp_header_t) + 8,
    (uint8_t[4]){ 0xff, 0xff, 0xff, 0xff },
    DHCP_CLIENT_PORT,
    DHCP_SERVER_PORT
  );
}
