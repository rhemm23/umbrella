#ifndef PCI_H
#define PCI_H

#define PCI_MAX_DEVICES 64

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_INVALID_VENDOR 0xFFFF

#include "type.h"

enum {
  PCI_NETWORK_CLASS = 0x02,
  PCI_DISPLAY_CLASS = 0x03
};

typedef struct {
  uint8_t bus;
  uint8_t slot;
  uint8_t func;
  uint8_t class;
  uint8_t subclass;
  uint16_t vendor;
  uint16_t device;
  uint32_t bars[6];
  uint8_t interrupt_line;
} pci_device_t;

void pci_init();

uint16_t pci_read_command(pci_device_t *device);
void pci_write_command(pci_device_t *device, uint16_t value);

pci_device_t * pci_find_device_by_class(uint8_t class);

#endif
