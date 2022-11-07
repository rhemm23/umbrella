#include "ethernet.h"
#include "rtl8139.h"
#include "string.h"
#include "arp.h"
#include "net.h"
#include "log.h"
#include "ip4.h"

static arp_protocol_entry_set_t arp_table[ARP_MAX_SUPPORTED_PROTOCOLS];
static int arp_table_cnt = 0;

static arp_protocol_entry_set_t * arp_lookup_or_add_protocol(uint16_t protocol_type, uint8_t protocol_addr_len) {

  for (int i = 0; i < arp_table_cnt; i++) {
    if (arp_table[i].protocol_type == protocol_type) {
      return &arp_table[i];
    }
  }

  arp_protocol_entry_set_t *entry_set = &arp_table[arp_table_cnt++];
  entry_set->num_entries = 0;
  entry_set->protocol_type = protocol_type;
  entry_set->protocol_addr_len = protocol_addr_len;

  return entry_set;
}

static void arp_add_entry(
  arp_protocol_entry_set_t *entry_set,
  uint8_t *protocol_addr,
  uint8_t *hardware_addr
) {

  for (int i = 0; i < entry_set->num_entries; i++) {
    int does_match = 1;
    for (int j = 0; j < entry_set->protocol_addr_len; j++) {
      if (protocol_addr[j] != entry_set->entries[i].protocol_addr[j]) {
        does_match = 0;
        break;
      }
    }
    if (does_match) {
      return;
    }
  }

  arp_entry_t *entry = &entry_set->entries[entry_set->num_entries++];

  entry->hardware_type = ARP_HARDWARE_ETHERNET;
  entry->hardware_addr_len = 6;

  memcpy(entry->hardware_addr, hardware_addr, 6);
  memcpy(entry->protocol_addr, protocol_addr, entry_set->protocol_addr_len);
}

void arp_add_ip4_addr(uint8_t *hardware_addr, uint8_t *ip_addr) {
  arp_add_entry(
    arp_lookup_or_add_protocol(ETHERNET_TYPE_IP4, 4),
    ip_addr,
    hardware_addr
  );
}

int arp_lookup(
  uint16_t protocol_type,
  uint8_t *protocol_addr,
  uint8_t protocol_addr_len,
  uint8_t *hardware_addr,
  uint8_t *hardware_addr_len
) {
  for (int i = 0; i < arp_table_cnt; i++) {
    if (arp_table[i].protocol_type == protocol_type) {
      for (int j = 0; j < arp_table[i].num_entries; j++) {
        int does_match = 1;
        for (int k = 0; k < arp_table[i].protocol_addr_len; k++) {
          if (protocol_addr[k] != arp_table[i].entries[j].protocol_addr[k]) {
            does_match = 0;
            break;
          }
        }
        if (does_match) {
          *hardware_addr_len = arp_table[i].entries[j].hardware_addr_len;
          for (int k = 0; k < arp_table[i].entries[j].hardware_addr_len; k++) {
            hardware_addr[k] = arp_table[i].entries[j].hardware_addr[k];
          }
          return 1;
        }
      }
    }
  }
  return 0;
}

static int is_protocol_type_valid(uint16_t protocol_type) {
  return protocol_type != 0xFFFF;
}

static int is_operation_valid(uint16_t operation) {
  return (operation > 0) && (operation < 26);
}

static int is_hardware_type_valid(uint16_t hardware_type) {
  return (hardware_type > 0) &&
         (hardware_type < 39 || hardware_type > 255) &&
         (hardware_type < 258);
}

void arp_handle_packet(uint8_t *packet, uint16_t packet_len) {

  arp_header_t *header = (arp_header_t*)packet;

  uint16_t operation = ntohs(header->operation);
  uint16_t hardware_type = ntohs(header->hardware_type);
  uint16_t protocol_type = ntohs(header->protocol_type);

  // Verify the packet data is valid
  if (!is_operation_valid(operation)) {
    log_info("(ARP) - Invalid Operation");
    return;
  }
  if (!is_hardware_type_valid(hardware_type)) {
    log_info("(ARP) - Invalid Hardware Type");
    return;
  }
  if (!is_protocol_type_valid(protocol_type)) {
    log_info("(ARP) - Invalid Protocol Type");
    return;
  }
  if (packet_len < (sizeof(arp_header_t) + (header->hardware_addr_len * 2) + (header->protocol_addr_len * 2))) {
    log_info("(ARP) - Invalid Packet Size");
    return;
  }

  uint8_t sender_protocol_addr[header->protocol_addr_len];
  uint8_t sender_hardware_addr[header->hardware_addr_len];
  
  uint8_t new_protocol_addr[header->protocol_addr_len];
  uint8_t new_hardware_addr[header->hardware_addr_len];

  int index = sizeof(arp_header_t);
  memcpy(sender_hardware_addr, &packet[index], header->hardware_addr_len);
  index += header->hardware_addr_len;
  memcpy(sender_protocol_addr, &packet[index], header->protocol_addr_len);
  index += header->protocol_addr_len;
  memcpy(new_hardware_addr, &packet[index], header->hardware_addr_len);
  index += header->hardware_addr_len;
  memcpy(new_protocol_addr, &packet[index], header->protocol_addr_len);

  // Process the data
  if (operation == ARP_OP_REPLY) {
    arp_protocol_entry_set_t *entry_set = arp_lookup_or_add_protocol(protocol_type, header->protocol_addr_len);
    arp_add_entry(entry_set, new_protocol_addr, new_hardware_addr);
  } else if (operation == ARP_OP_REQUEST) {
    int does_match = 1;
    for (int i = 0; i < 4; i++) {
      if (ip_address[i] != new_protocol_addr[i]) {
        does_match = 0;
        break;
      }
    }
    if (does_match) {
      arp_send_ip4_packet(ARP_OP_REPLY, sender_hardware_addr, sender_protocol_addr);
    }
  }
}

void arp_send_ip4_packet(uint16_t operation, uint8_t *target_hardware_addr, uint8_t *target_protocol_addr) {

  arp_header_t header;
  uint16_t packet_len = sizeof(arp_header_t) + 20;
  uint8_t packet[packet_len];

  header.hardware_type = htons(0x01);
  header.protocol_type = htons(ETHERNET_TYPE_IP4);
  header.hardware_addr_len = 0x06;
  header.protocol_addr_len = 0x04;
  header.operation = htons(operation);

  memcpy(packet, &header, sizeof(arp_header_t));
  int index = sizeof(arp_header_t);

  memcpy(&packet[index], mac_addr, 6);
  index += 6;
  memcpy(&packet[index], ip_address, 4);
  index += 4;
  memcpy(&packet[index], target_hardware_addr, 6);
  index += 6;
  memcpy(&packet[index], target_protocol_addr, 4);

  ethernet_send_packet(packet, packet_len, target_hardware_addr, ETHERNET_TYPE_ARP);
}

void arp_init() {

  uint8_t broadcast_mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  uint8_t broadcast_ip[4] = { 0xff, 0xff, 0xff, 0xff };

  arp_add_ip4_addr(broadcast_mac, broadcast_ip);
}
