#include "ethernet.h"
#include "rtl8139.h"
#include "string.h"
#include "arp.h"
#include "net.h"
#include "log.h"
#include "ip4.h"
#include "dns.h"
#include "dhcp.h"

static arp_queued_ip4_packet_t arp_queued_ip4_packets[ARP_MAX_IP4_QUEUED_PACKETS];

static arp_protocol_entry_set_t arp_table[ARP_MAX_SUPPORTED_PROTOCOLS];
static int arp_table_cnt = 0;

static uint8_t broadcast_mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static uint8_t broadcast_ip[4] = { 0xff, 0xff, 0xff, 0xff };

static void arp_send_queued_ip4_packets(uint8_t *dst_ip_address, uint8_t *mac_addr) {
  for (int i = 0; i < ARP_MAX_IP4_QUEUED_PACKETS; i++) {
    if (memcmp(arp_queued_ip4_packets[i].ip_address, dst_ip_address, 4)) {
      ethernet_send_packet(arp_queued_ip4_packets[i].packet, arp_queued_ip4_packets[i].packet_len, mac_addr, ETHERNET_TYPE_IP4);
      memset(arp_queued_ip4_packets[i].ip_address, 0x00, 4);
    }
  }
}

static void arp_queue_ip4_packet(uint8_t *dst_ip_address, uint8_t *packet, uint16_t packet_len) {

  // Determine if we need to send a request
  uint8_t need_send_request = 1;
  for (int i = 0; i < ARP_MAX_IP4_QUEUED_PACKETS; i++) {
    if (memcmp(arp_queued_ip4_packets[i].ip_address, dst_ip_address, 4)) {
      need_send_request = 0;
      break;
    }
  }

  // Store in static array
  uint8_t invalid_ip[4] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t found_spot = 0;

  for (int i = 0; i < ARP_MAX_IP4_QUEUED_PACKETS; i++) {
    if (memcmp(arp_queued_ip4_packets[i].ip_address, invalid_ip, 4)) {
      memcpy(arp_queued_ip4_packets[i].ip_address, dst_ip_address, 4);
      memcpy(arp_queued_ip4_packets[i].packet, packet, packet_len);
      arp_queued_ip4_packets[i].packet_len = packet_len;
      found_spot = 1;
      break;
    }
  }

  // Log error if no space for queued packet, otherwise send request if needed
  if (!found_spot) {
    log_error("(ARP) Reached max allowed queued IP4 packets, discarding packet");
  } else if (need_send_request) {
    arp_send_ip4_request(dst_ip_address);
  }
}

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
  uint8_t *hardware_addr,
  uint8_t *hardware_addr_len
) {
  for (int i = 0; i < arp_table_cnt; i++) {
    if (arp_table[i].protocol_type == protocol_type) {
      for (int j = 0; j < arp_table[i].num_entries; j++) {
        if (memcmp(protocol_addr, arp_table[i].entries[j].protocol_addr, arp_table[i].protocol_addr_len)) {
          *hardware_addr_len = arp_table[i].entries[j].hardware_addr_len;
          memcpy(hardware_addr, arp_table[i].entries[j].hardware_addr, arp_table[i].entries[j].hardware_addr_len);
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
    log_warn("(ARP) - Invalid Operation");
    return;
  }
  if (!is_hardware_type_valid(hardware_type)) {
    log_warn("(ARP) - Invalid Hardware Type");
    return;
  }
  if (!is_protocol_type_valid(protocol_type)) {
    log_warn("(ARP) - Invalid Protocol Type");
    return;
  }
  if (packet_len < (sizeof(arp_header_t) + (header->hardware_addr_len * 2) + (header->protocol_addr_len * 2))) {
    log_warn("(ARP) - Invalid Packet Size");
    return;
  }

  uint8_t sender_protocol_addr[header->protocol_addr_len];
  uint8_t sender_hardware_addr[header->hardware_addr_len];
  
  uint8_t target_protocol_addr[header->protocol_addr_len];
  uint8_t target_hardware_addr[header->hardware_addr_len];

  int index = sizeof(arp_header_t);
  memcpy(sender_hardware_addr, &packet[index], header->hardware_addr_len);
  index += header->hardware_addr_len;
  memcpy(sender_protocol_addr, &packet[index], header->protocol_addr_len);
  index += header->protocol_addr_len;
  memcpy(target_hardware_addr, &packet[index], header->hardware_addr_len);
  index += header->hardware_addr_len;
  memcpy(target_protocol_addr, &packet[index], header->protocol_addr_len);

  if (hardware_type != ARP_HARDWARE_ETHERNET) {
    log_warn("(ARP) Ignored ARP packet with unsupported hardware type");
    return;
  }

  switch (operation) {
    case ARP_OP_REPLY:
      if (memcmp(target_hardware_addr, mac_addr, 6)) {
        log_info("(ARP) Received Response to Request");
        arp_protocol_entry_set_t *entry_set = arp_lookup_or_add_protocol(protocol_type, header->protocol_addr_len);
        arp_add_entry(entry_set, sender_protocol_addr, sender_hardware_addr);
        arp_send_queued_ip4_packets(sender_protocol_addr, sender_hardware_addr);
      }
      break;

    case ARP_OP_REQUEST:
      if (protocol_type == ETHERNET_TYPE_IP4) {
        if (memcmp(target_protocol_addr, ip_address, 4)) {
          arp_send_ip4_reply(sender_hardware_addr, sender_protocol_addr);
        }
      } else {
        log_warn("(ARP) Ignored ARP request with unsupported protocol type");
      }
      break;
  }
}

void arp_handle_ip4_packet_send(uint8_t *dst_ip_address, uint8_t *packet, uint16_t packet_len) {
  uint8_t *mac, *mac_len;
  if (arp_lookup(ETHERNET_TYPE_IP4, dst_ip_address, mac, mac_len)) {
    ethernet_send_packet(packet, packet_len, mac, ETHERNET_TYPE_IP4);
  } else {
    arp_queue_ip4_packet(dst_ip_address, packet, packet_len);
  }
}

void arp_send_ip4_request(uint8_t *target_ip4_addr) {
  arp_send_ip4_packet(ARP_OP_REQUEST, broadcast_mac, target_ip4_addr);
  log_info("(ARP) Sending Request for IPv4");
}

void arp_send_ip4_reply(uint8_t *target_mac_addr, uint8_t *target_ip4_addr) {
  arp_send_ip4_packet(ARP_OP_REPLY, target_mac_addr, target_ip4_addr);
  log_info("(ARP) Sending Reply to Request for IPv4");
}

void arp_send_ip4_packet(uint16_t operation, uint8_t *target_mac_addr, uint8_t *target_ip4_addr) {

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
  memcpy(&packet[index], target_mac_addr, 6);
  index += 6;
  memcpy(&packet[index], target_ip4_addr, 4);

  ethernet_send_packet(packet, packet_len, target_mac_addr, ETHERNET_TYPE_ARP);
}

void arp_init() {
  arp_add_ip4_addr(broadcast_mac, broadcast_ip);
}
