#include "ethernet.h"
#include "arp.h"
#include "net.h"
#include "log.h"

arp_entry_t arp_table[ARP_MAX_ENTRIES];

int arp_current_index = 0;
int arp_entry_cnt = 0;

void arp_lookup_add(uint8_t ip[4], uint8_t mac[6]) {
  for (int i = 0; i < 4; i++) {
    arp_table[arp_current_index].ip_addr[i] = ip[i];
  }
  for (int i = 0; i < 6; i++) {
    arp_table[arp_current_index].mac_addr[i] = mac[i];
  }
  if (arp_entry_cnt < ARP_MAX_ENTRIES) {
    arp_entry_cnt++;
  }
  arp_current_index = (arp_current_index + 1) % ARP_MAX_ENTRIES;
}

uint8_t * arp_lookup(uint8_t ip[4]) {
  for (int i = 0; i < arp_entry_cnt; i++) {
    int matches = 1;
    for (int j = 0; j < 4; j++) {
      if (ip[j] != arp_table[i].ip_addr[j]) {
        matches = 0;
        break;
      }
    }
    if (matches) {
      return &arp_table[i].mac_addr[0];
    }
  }
  return (uint8_t*)0;
}

void arp_handle_packet(uint8_t *packet, uint16_t packet_len) {

  arp_header_t *header = (arp_header_t*)packet;

  uint16_t operation = ntohs(header->operation);
  uint16_t hardware_type = ntohs(header->hardware_type);
  uint16_t protocol_type = ntohs(header->protocol_type);

  switch (hardware_type) {
    case ARP_HARDWARE_ETHERNET:
      switch (protocol_type) {
        case ETHERNET_TYPE_IP4:
          switch (operation) {
            case ARP_REQUEST:
              log_info("(ARP) Received Request");
              break;

            case ARP_REPLY:
              log_info("(ARP) Received Reply");
              break;

            default:
              log_info("(ARP) Unsupported operation");
              break;
          }
          break;

        default:
          log_info("(ARP) Unsupported protocol type");
          break;
      }
      break;

    default:
      log_info("(ARP) Unsupported hardware type");
      break;
  }
}

void arp_send_packet() {

}

void arp_init() {

  // Add broadcast mac
  arp_lookup_add((uint8_t[4]){ 0xff, 0xff, 0xff, 0xff }, (uint8_t[6]){ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff });
}
