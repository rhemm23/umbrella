#ifndef TCP_H
#define TCP_H

#include "type.h"

typedef struct {
  uint16_t src_port;
  uint16_t dst_port;
  uint32_t sqc_number;
  uint32_t ack_number;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t rsv1 : 4;
  uint8_t data_offset : 4;
  uint8_t fin : 1;
  uint8_t syn : 1;
  uint8_t rst : 1;
  uint8_t psh : 1;
  uint8_t ack : 1;
  uint8_t urg : 1;
  uint8_t ece : 1;
  uint8_t cwr : 1;
#else
  uint8_t data_offset : 4;
  uint8_t rsv1 : 4;
  uint8_t cwr : 1;
  uint8_t ece : 1;
  uint8_t urg : 1;
  uint8_t ack : 1;
  uint8_t psh : 1;
  uint8_t rst : 1;
  uint8_t syn : 1;
  uint8_t fin : 1;
#endif
  uint16_t win_size;
  uint16_t checksum;
  uint16_t urgent_ptr;
} __attribute__ ((packed)) tcp_header_t;

void tcp_handle_packet(uint8_t *packet, uint16_t packet_len);

#endif