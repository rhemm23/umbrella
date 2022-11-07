#include "ethernet.h"
#include "ip4.h"
#include "net.h"
#include "log.h"
#include "arp.h"
#include "udp.h"

uint8_t ip_address[4] = { 0, 0, 0, 0 };

void ip4_handle_packet(uint8_t *packet, uint16_t packet_len) {

  ip4_header_t *header = (ip4_header_t*)packet;
  uint8_t header_length = header->ihl * sizeof(uint32_t);
  uint8_t *packet_data = packet + header_length;
  uint16_t packet_length = packet_len - header_length;

  switch (header->protocol) {
    case IP_PROTO_TCP:
      log_info("(IP4) TCP packet received");
      break;

    case IP_PROTO_UDP:
      udp_handle_packet(packet_data, packet_length);
      break;

    default:
      log_info("(IP4) Unknown IP4 protocol");
      break;
  }
}

uint16_t ip4_calculate_checksum(ip4_header_t *header) {
  uint32_t sum = 0;
  uint16_t *values = (uint16_t*)header;
  for (int i = 0; i < sizeof(ip4_header_t) / 2; i++) {
    sum += htons(values[i]);
  }
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }
  return ~sum;
}

void ip4_send_packet(uint8_t *packet, uint16_t packet_len, uint8_t dst_ip_addr[4], uint8_t protocol) {

  // Default values
  ip4_header_t header;
  header.ihl = sizeof(ip4_header_t) / 4;
  header.version = 4;
  header.ecn = 0;
  header.dscp = 0;
  header.total_length = htons(packet_len + sizeof(ip4_header_t));
  header.identification = 0;
  header.flags = 0;
  header.fragment_offset_high = 0;
  header.fragment_offset_low = 0;
  header.ttl = IP_DEFAULT_TTL;
  header.protocol = protocol;
  header.header_checksum = 0;

  // Copy ip addresses
  for (int i = 0; i < 4; i++) {
    header.src_ip_addr[i] = ip_address[i];
    header.dst_ip_addr[i] = dst_ip_addr[i];
  }

  // Calculate checksum
  header.header_checksum = htons(ip4_calculate_checksum(&header));

  uint8_t packet_data[packet_len + sizeof(ip4_header_t)];
  uint8_t *packet_header = (uint8_t*)&header;

  // Copy header data
  for (int i = 0; i < sizeof(ip4_header_t); i++) {
    packet_data[i] = packet_header[i];
  }
  for (int i = 0; i < packet_len; i++) {
    packet_data[sizeof(ip4_header_t) + i] = packet[i];
  }

  uint8_t *mac, *mac_len;
  if (arp_lookup(ETHERNET_TYPE_IP4, dst_ip_addr, 4, mac, mac_len)) {
    ethernet_send_packet(&packet_data[0], packet_len + sizeof(ip4_header_t), mac, ETHERNET_TYPE_IP4);
  }
}
