#ifndef _AOS_PCI_H
#define _AOS_PCI_H

#include <aos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PCI_CMD_FIND_DEVICE = 1,
    PCI_CMD_READ_CONFIG,
    PCI_CMD_WRITE_CONFIG
} pci_cmd_t;

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    
    uint32_t bar[6];
} pci_device_info_t;

#define PCI_ERR_OK                 DRV_ERR_OK
#define PCI_ERR_NOCOMM             DRV_ERR_NOCOMM
#define PCI_ERR_NOTFOUND           DRV_ERR_NOTFOUND
#define PCI_ERR_UNKNOWN            DRV_ERR_UNKNOWN

int pci_find_device(uint8_t class_code, uint8_t subclass, pci_device_info_t* out_info);
uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif /* _AOS_PCI_H */