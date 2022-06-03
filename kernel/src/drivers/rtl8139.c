#include "ethernet.h"
#include "rtl8139.h"
#include "port.h"
#include "pci.h"
#include "irq.h"
#include "log.h"
#include "sys.h"

uint8_t mac_addr[6];

pci_device_t *pci_rtl8139_device;
rtl8139_device_t rtl8139_device;

uint8_t rx_buffer[9708] __attribute__((aligned(32)));
uint16_t rx_offset;

static uint8_t tx_buf[4] = {
  0x20,
  0x24,
  0x28,
  0x2C
};

static uint8_t tx_sts[4] = {
  0x10,
  0x14,
  0x18,
  0x1C
};

static uint8_t tx_buffers[4][RTL8139_MAX_TX_LEN];
static uint8_t tx_cur = 0;

void rtl8139_send_packet(uint8_t *packet, uint16_t packet_len) {

  // Validate length
  if (packet_len > RTL8139_MAX_TX_LEN) {
    log_warn("Cannot transmit packet with size greater than 1792 bytes");
    return;
  }

  // Copy packet data to buffer
  for (int i = 0; i < packet_len; i++) {
    tx_buffers[tx_cur][i] = packet[i];
  }

  // Transmit
  port_write_dword(rtl8139_device.io_base + tx_buf[tx_cur], (uint32_t)&tx_buffers[tx_cur][0]);
  port_write_dword(rtl8139_device.io_base + tx_sts[tx_cur], (uint32_t)packet_len);
  tx_cur = (tx_cur + 1) % 4;
}

void rtl8139_receive_packet() {

  uint16_t *packet = (uint16_t*)(&rx_buffer[0] + rx_offset);
  uint16_t packet_status = packet[0];
  uint16_t packet_length = packet[1];

  uint16_t packet_error = ((packet_status >> 1) & 0x000F);
  uint16_t packet_valid = (packet_status & 0x0001);

  // Handle packet if valid
  if (!packet_error && packet_valid) {
    ethernet_handle_packet((uint8_t*)&packet[2], packet_length);
  } else {
    log_info("Bad packet received, discarding");
  }

  // Update the buffer offset
  rx_offset = (rx_offset + packet_length + 0x07) & RTL8139_RX_READ_POINTER_MASK;
  if (rx_offset > 8192) {
    rx_offset -= 8192;
  }
  port_write_word(rtl8139_device.io_base + RTL8139_CPR, rx_offset - 0x10);
}

void rtl8139_interrupt_handler(register_t *reg) {
  uint16_t status = port_read_word(rtl8139_device.io_base + RTL8139_ISR);
  if (status & RTL8139_ROK) {
    rtl8139_receive_packet();
  }
  if (status & RTL8139_TOK) {
    log_info("Packet transmitted");
  }
  port_write_word(rtl8139_device.io_base + RTL8139_ISR, 0x05);
}

void rtl8139_read_mac_addr(uint8_t *storage) {
  uint32_t low_bytes = port_read_dword(rtl8139_device.io_base);
  uint16_t high_bytes = port_read_word(rtl8139_device.io_base + 0x04);
  storage[0] = (uint8_t)(low_bytes & 0xFF);
  storage[1] = (uint8_t)((low_bytes >> 8) & 0xFF);
  storage[2] = (uint8_t)((low_bytes >> 16) & 0xFF);
  storage[3] = (uint8_t)((low_bytes >> 24) & 0xFF);
  storage[4] = (uint8_t)(high_bytes & 0xFF);
  storage[5] = (uint8_t)((high_bytes >> 8) & 0xFF);
}

void rtl8139_init() {

  pci_rtl8139_device = pci_find_device_by_class(PCI_NETWORK_CLASS);

  if (pci_rtl8139_device == 0 ||
      pci_rtl8139_device->vendor != RTL8139_VENDOR_ID ||
      pci_rtl8139_device->device != RTL8139_DEVICE_ID) {
    log_info("RTL8139 device not found, skipping initialization");
    return;
  }

  rtl8139_device.bar_type = pci_rtl8139_device->bars[0] & 0x01;
  rtl8139_device.io_base = pci_rtl8139_device->bars[0] & ~0x03;
  rtl8139_device.mem_base = pci_rtl8139_device->bars[0] & ~0x0F;

  // Enable PCI bus mastering
  uint32_t command_value = pci_read_command(pci_rtl8139_device);
  pci_write_command(pci_rtl8139_device, command_value | 0x04);

  // Power on
  port_write_byte(rtl8139_device.io_base + RTL8139_CONFIG_1, 0x00);

  // Software reset
  port_write_byte(rtl8139_device.io_base + RTL8139_COMMAND, 0x10);
  while ((port_read_byte(rtl8139_device.io_base + RTL8139_COMMAND) & 0x10) != 0);

  // Setup receive buffer
  port_write_dword(rtl8139_device.io_base + RTL8139_RBSTART, (uint32_t)&rx_buffer[0]);

  // Accept transmit ok and receive ok interrupts
  port_write_word(rtl8139_device.io_base + RTL8139_IMR, 0x0005);

  // Accept all packet types
  port_write_dword(rtl8139_device.io_base + 0x44, 0xF | (1 << 7));

  // Enable receive / transmit
  port_write_byte(rtl8139_device.io_base + RTL8139_CMD, 0x0C);

  // Setup interrupt handler
  irq_register_handler(0x20 + pci_rtl8139_device->interrupt_line, rtl8139_interrupt_handler);

  // Save mac address
  rtl8139_read_mac_addr(&mac_addr[0]);

  log_info("RTL8139 device setup");
}
