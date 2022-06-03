#include "ethernet.h"
#include "rtl8139.h"
#include "net.h"
#include "log.h"
#include "arp.h"
#include "ip4.h"

void ethernet_handle_packet(uint8_t *packet, uint16_t packet_len) {

  ethernet_header_t *header = (ethernet_header_t*)packet;
  uint8_t *packet_data = packet + sizeof(ethernet_header_t);
  uint16_t packet_type = ntohs(header->type);
  uint16_t packet_length = packet_len - sizeof(ethernet_header_t);

  switch (packet_type) {
    case ETHERNET_TYPE_ARP:
      arp_handle_packet(packet_data, packet_length);
      break;

    case ETHERNET_TYPE_IP4:
      ip4_handle_packet(packet_data, packet_length);
      break;

    case ETHERNET_TYPE_IP6:
      log_info("(IP6) Packet Received");
      break;
  }
}

void ethernet_send_packet(uint8_t *packet, uint16_t packet_len, uint8_t dst_mac[6], uint16_t type) {

  // Check packet size
  if (packet_len > (RTL8139_MAX_TX_LEN - sizeof(ethernet_header_t))) {
    log_warn("Packet size too large");
    return;
  }

  uint8_t packet_data[RTL8139_MAX_TX_LEN];

  // Copy mac addresses
  for (int i = 0; i < 6; i++) {
    packet_data[i] = dst_mac[i];
    packet_data[i + 6] = mac_addr[i];
  }

  // Copy ethertype
  packet_data[12] = (uint8_t)(type >> 8);
  packet_data[13] = (uint8_t)(type & 0xFF);

  // Copy packet info
  for (int i = 0; i < packet_len; i++) {
    packet_data[i + sizeof(ethernet_header_t)] = packet[i];
  }

  rtl8139_send_packet(&packet_data[0], packet_len + sizeof(ethernet_header_t));
}