#include "string.h"
#include "dhcp.h"
#include "dns.h"
#include "net.h"
#include "udp.h"
#include "log.h"

static uint16_t identification = 0;

void dns_handle_packet(uint8_t *packet, uint16_t packet_len) {
  log_info("(DNS) Received Response");
}

void dns_query_ip_addr(char *hostname) {

  dns_packet_t packet;
  memset(&packet, 0x00, sizeof(dns_packet_t));
  packet.identification = identification++;
  packet.flags.rcr_dsd = 1;
  packet.number_of_questions = htons(1);

  // Hostname string size
  char *temp = hostname;
  int size = 0;

  while (*temp++) {
    size++;
  }

  uint8_t question[size + 2];
  question[size + 1] = 0;

  uint8_t index = 0;
  uint8_t segment_size = 0;

  while (hostname[index]) {
    if (hostname[index] == '.') {
      question[index - segment_size] = segment_size;
      segment_size = 0;
    } else {
      question[index + 1] = hostname[index];
      segment_size++;
    }
    index++;
  }
  question[index - segment_size] = segment_size;

  uint16_t packet_len = sizeof(dns_packet_t) + size + 6;
  uint8_t packet_data[packet_len];

  memcpy(packet_data, &packet, sizeof(dns_packet_t));
  index = sizeof(dns_packet_t);
  memcpy(&packet_data[index], question, size + 2);
  index += size + 2;

  packet_data[index++] = 0x00;
  packet_data[index++] = DNS_QUERY_TYPE_A;

  packet_data[index++] = 0x00;
  packet_data[index++] = DNS_QUERY_CLASS_IN;

  log_info("(DNS) Sending Query");

  udp_send_packet(
    packet_data,
    packet_len,
    domain_server_ip,
    DNS_CLIENT_PORT,
    DNS_SERVER_PORT
  );
}
