#ifndef DNS_H
#define DNS_H

#include "type.h"

#define DNS_QUERY_TYPE_A 0x01
#define DNS_QUERY_TYPE_NS 0x02

#define DNS_QUERY_CLASS_IN 0x01

#define DNS_CLIENT_PORT 53
#define DNS_SERVER_PORT 53

typedef struct dns_packet {
  uint16_t identification;
  struct {
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint8_t rcode : 4;
    uint8_t rsv : 3;
    uint8_t rcr_avl : 1;
    uint8_t rcr_dsd : 1;
    uint8_t truncation : 1;
    uint8_t auth_ans : 1;
    uint8_t opcode : 4;
    uint8_t msg_type : 1;
  #else
    uint8_t msg_type : 1;
    uint8_t opcode : 4;
    uint8_t auth_ans : 1;
    uint8_t truncation : 1;
    uint8_t rcr_dsd : 1;
    uint8_t rcr_avl : 1;
    uint8_t rsv : 3;
    uint8_t rcode : 4;
  #endif
  } flags;
  uint16_t number_of_questions;
  uint16_t number_of_answer_rrs;
  uint16_t number_of_authority_rrs;
  uint16_t number_of_additional_rrs;
} __attribute__((packed)) dns_packet_t;

void dns_query_ip_addr(char *hostname);
void dns_handle_packet(uint8_t *packet, uint16_t packet_len);

#endif
