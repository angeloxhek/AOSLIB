#include <aos/pci.h>
#include <aos/ipc.h>
#include <aos/driver.h>
#include <aos/syscalls.h>
#include <string.h>

static apid_t pci_driver_pid = 0;

#define ensure_pci_init() { if (pci_driver_pid == 0) pci_driver_pid = get_driver_pid(DT_PCI); }

int pci_find_device(uint8_t class_code, uint8_t subclass, pci_device_info_t* out_info) {
    if (!out_info) return PCI_ERR_NOTFOUND;
    ensure_pci_init();
    if (pci_driver_pid == 0) return PCI_ERR_NOCOMM;

    void* shm_vaddr;
    uint64_t shm_id = shm_alloc(sizeof(pci_device_info_t), &shm_vaddr);
    if (!shm_id) return PCI_ERR_NOTFOUND;
    shm_allow(shm_id, pci_driver_pid);

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    req.type = MSG_TYPE_PCI;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = PCI_CMD_FIND_DEVICE;
    req.param2 = class_code;
    req.param3 = subclass;
    *(uint64_t*)(req.data) = shm_id;

    ipc_send(pci_driver_pid, &req);
    ipc_recv_ex(pci_driver_pid, MSG_TYPE_PCI, MSG_SUBTYPE_RESPONSE, req.id, MSG_FLAG_NONE, &resp);

    int res = resp.param1;
    if (res == PCI_ERR_OK) {
        memcpy(out_info, shm_vaddr, sizeof(pci_device_info_t));
    }

    shm_free(shm_id);
    return res;
}

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    ensure_pci_init();
    if (pci_driver_pid == 0) return 0xFFFFFFFF;

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    req.type = MSG_TYPE_PCI;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = PCI_CMD_READ_CONFIG;
    req.param2 = (bus << 16) | (slot << 8) | func;
    req.param3 = offset;

    ipc_send(pci_driver_pid, &req);
    ipc_recv_ex(pci_driver_pid, MSG_TYPE_PCI, MSG_SUBTYPE_RESPONSE, req.id, MSG_FLAG_NONE, &resp);

    if (resp.param1 == PCI_ERR_OK) {
        return (uint32_t)resp.param2;
    }
    return 0xFFFFFFFF;
}

void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    ensure_pci_init();
    if (pci_driver_pid == 0) return;

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    req.type = MSG_TYPE_PCI;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = PCI_CMD_WRITE_CONFIG;
    req.param2 = (bus << 16) | (slot << 8) | func;
    req.param3 = offset;
    *(uint32_t*)(req.data) = value;

    ipc_send(pci_driver_pid, &req);
    ipc_recv_ex(pci_driver_pid, MSG_TYPE_PCI, MSG_SUBTYPE_RESPONSE, req.id, MSG_FLAG_NONE, &resp);
}