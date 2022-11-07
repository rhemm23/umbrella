#ifndef DNS_H
#define DNS_H

#include "type.h"

#define DNS_QUERY_TYPE_A 0x01
#define DNS_QUERY_TYPE_NS 0x02

#define DNS_QUERY_CLASS_IP 0x01

typedef struct dns_packet {
  uint16_t identification;
  uint16_t flags;
  uint16_t number_of_questions;
  uint16_t number_of_answer_rrs;
  uint16_t number_of_authority_rrs;
  uint16_t number_of_additional_rrs;
} dns_packet_t;

void dns_query_ip_addr(char *hostname);

#endif
