#include "kalloc.h"
#include "port.h"
#include "pci.h"

uint8_t num_pci_devices = 0;
pci_device_t **pci_devices;

static uint32_t pci_calc_address(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
  return ((uint32_t)bus << 16)       |
         ((uint32_t)slot << 11)      |
         ((uint32_t)func << 8)       |
         ((uint32_t)(offset & 0xFC)) |
         ((uint32_t)0x80000000);
}

uint32_t pci_read_config_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
  port_write_dword(PCI_CONFIG_ADDRESS, pci_calc_address(bus, slot, func, offset));
  return port_read_dword(PCI_CONFIG_DATA);
}

uint16_t pci_read_config_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
  uint32_t value = pci_read_config_dword(bus, slot, func, offset);
  return (uint16_t)((value >> ((offset & 2) * 8)) & 0xFFFF);
}

uint8_t pci_read_class(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t class = pci_read_config_word(bus, slot, func, 0x0A);
  return (class & 0xFF00) >> 8;
}

uint8_t pci_read_subclass(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t class = pci_read_config_word(bus, slot, func, 0x0A);
  return class & 0x00FF;
}

uint8_t pci_read_interrupt_line(uint8_t bus, uint8_t slot, uint8_t func) {
  uint16_t result = pci_read_config_word(bus, slot, func, 0x3C);
  return result & 0x00FF;
}

uint16_t pci_read_vendor(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_read_config_word(bus, slot, func, 0x00);
}

uint16_t pci_read_device(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_read_config_word(bus, slot, func, 0x02);
}

uint16_t pci_read_command(pci_device_t *device) {
  return pci_read_config_word(device->bus, device->slot, device->func, 0x04);
}

void pci_write_command(pci_device_t *device, uint16_t value) {
  port_write_dword(PCI_CONFIG_ADDRESS, pci_calc_address(device->bus, device->slot, device->func, 0x04));
  port_write_word(PCI_CONFIG_DATA, value);
}

pci_device_t * pci_find_device_by_class(uint8_t class) {
  for (uint8_t i = 0; i < num_pci_devices; i++) {
    if (pci_devices[i]->class == class) {
      return pci_devices[i];
    }
  }
  return 0;
}

void pci_map_devices() {
  pci_devices = (pci_device_t**)kalloc(PCI_MAX_DEVICES * sizeof(pci_device_t*));
  for (uint8_t bus = 0; bus < 256; bus++) {
    for (uint8_t slot = 0; slot < 32; slot++) {
      for (uint8_t func = 0; func < 8; func++) {
        uint16_t vendor = pci_read_vendor(bus, slot, func);
        if (vendor != PCI_INVALID_VENDOR) {
          pci_devices[num_pci_devices] = (pci_device_t*)kalloc(sizeof(pci_device_t));
          pci_devices[num_pci_devices]->bus = bus;
          pci_devices[num_pci_devices]->slot = slot;
          pci_devices[num_pci_devices]->func = func;
          pci_devices[num_pci_devices]->vendor = vendor;
          pci_devices[num_pci_devices]->device = pci_read_device(bus, slot, func);
          pci_devices[num_pci_devices]->class = pci_read_class(bus, slot, func);
          pci_devices[num_pci_devices]->subclass = pci_read_class(bus, slot, func);
          pci_devices[num_pci_devices]->interrupt_line = pci_read_interrupt_line(bus, slot, func);
          for (uint8_t i = 0; i < 6; i++) {
            pci_devices[num_pci_devices]->bars[i] = pci_read_config_dword(bus, slot, func, 0x10 + i * 0x04);
          }
          num_pci_devices++;
        }
      }
    }
    if (bus == 255) {
      break;
    }
  }
}

void pci_init() {
  pci_map_devices();
}