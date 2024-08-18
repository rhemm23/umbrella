#include "tcp.h"
#include "log.h"

void tcp_handle_packet(uint8_t *packet, uint16_t packet_len) {

  tcp_header_t *header = (tcp_header_t*)packet;

  uint8_t header_size = header->data_offset * sizeof(uint32_t);
  uint8_t *packet_data = packet + header_size;
  uint16_t packet_length = packet_len - header_size;

  log_info("(TCP) Packet Received");
}
