#ifndef RTL8139_H
#define RTL8139_H

#define RTL8139_RX_READ_POINTER_MASK (~3)
#define RTL8139_MAX_TX_LEN 0x700

#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define RTL8139_CONFIG_1 0x52
#define RTL8139_COMMAND 0x37
#define RTL8139_RBSTART 0x30
#define RTL8139_IMR 0x3C
#define RTL8139_ISR 0x3E
#define RTL8139_CMD 0x37
#define RTL8139_CPR 0x38
#define RTL8139_ROK 0x01
#define RTL8139_TOK 0x04

#include "type.h"

typedef struct {
  uint8_t bar_type;
  uint16_t io_base;
  uint32_t mem_base;
} rtl8139_device_t;

extern uint8_t mac_addr[6];

void rtl8139_init();
void rtl8139_send_packet(uint8_t *packet, uint16_t packet_len);

#endif
