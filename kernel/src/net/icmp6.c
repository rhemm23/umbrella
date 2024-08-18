#include "icmp6.h"
#include "log.h"

void icmp6_handle_packet(uint8_t *packet, uint16_t packet_len) {

  icmp6_header_t *header = (icmp6_header_t*)packet;

  // TODO
}
