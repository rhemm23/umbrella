#include "dhcp.h"
#include "udp.h"
#include "net.h"
#include "ip4.h"
#include "log.h"

void udp_handle_packet(uint8_t *packet, uint16_t packet_len) {

  udp_header_t *header = (udp_header_t*)packet;

  uint16_t src_port = ntohs(header->src_port);
  uint16_t dst_port = ntohs(header->dst_port);
  uint16_t length = ntohs(header->length);

  uint8_t *packet_data = packet + sizeof(udp_header_t);
  uint16_t packet_length = packet_len - sizeof(udp_header_t);

  if (src_port == DHCP_SERVER_PORT) {
    dhcp_handle_packet(packet_data, packet_length);
  } else {
    log_info("(UDP) Received packet");
  }
}

void udp_send_packet(uint8_t *packet, uint16_t packet_len, uint8_t dst_ip[4], uint16_t src_port, uint16_t dst_port) {

  udp_header_t header;
  header.src_port = htons(src_port);
  header.dst_port = htons(dst_port);
  header.length = htons(packet_len + sizeof(udp_header_t));
  header.checksum = 0; // <-- Could calculate in future

  uint8_t packet_data[packet_len + sizeof(udp_header_t)];
  uint8_t *packet_header = (uint8_t*)&header;

  // Copy header
  for (int i = 0; i < sizeof(udp_header_t); i++) {
    packet_data[i] = packet_header[i];
  }

  // Copy payload
  for (int i = 0; i < packet_len; i++) {
    packet_data[sizeof(udp_header_t) + i] = packet[i];
  }

  // Transmit
  ip4_send_packet(&packet_data[0], packet_len + sizeof(udp_header_t), dst_ip, IP_PROTO_UDP);
}
