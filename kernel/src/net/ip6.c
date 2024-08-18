#include "icmp6.h"
#include "ip6.h"
#include "ip4.h"
#include "udp.h"
#include "tcp.h"
#include "log.h"

void ip6_handle_packet(uint8_t *packet, uint16_t packet_len) {
  
  ip6_header_t *header = (ip6_header_t*)packet;
  uint8_t *packet_data = packet + sizeof(ip6_header_t);
  uint16_t packet_length = packet_len - sizeof(ip6_header_t);

  switch (header->next_header) {
    case IP_PROTO_TCP:
      tcp_handle_packet(packet_data, packet_length);
      break;

    case IP_PROTO_UDP:
      udp_handle_packet(packet_data, packet_length);
      break;

    case IP_PROTO_HOPOPT:
      log_info("(HOPOPT) Received Packet");
      break;

    case IP_PROTO_ICMP:
      log_info("(ICMP) Received Packet");
      break;

    case IP_PROTO_IGMP:
      log_info("(IGMP) Received Packet");
      break;

    case IP_PROTO_ICMPv6:
      icmp6_handle_packet(packet_data, packet_length);
      break;

    default:
      log_info("(IP6) Unknown IP6 protocol");
      logx(header->next_header);
      break;
  }
}
