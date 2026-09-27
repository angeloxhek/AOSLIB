#ifndef AOS_DRIVER_H
#define AOS_DRIVER_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DT_NONE = 0,
    DT_AUTH,
    DT_VFS,
    DT_INIT,
    DT_VIDEO,
    DT_WND,
    DT_INPUT,
    DT_USER = 100
} driver_type_t;

#define CAN_PRINT (1 << 0)
#define CAN_REGISTER_KERNEL_DRIVERS (1 << 1)
#define KERNEL_PANIC (1 << 2)
#define FSGSBASE (1 << 0)

typedef struct {
    uint64_t uptime;
    uint64_t fs_base;
    uint64_t gs_base;
    uint64_t kernel_gs_base;
    uint32_t flags;
    uint16_t cpu_flags;
} system_info_t;

typedef enum {
    DISK_TYPE_UNKNOWN = 0,
    DISK_TYPE_IDE,
    DISK_TYPE_AHCI,
    DISK_TYPE_NVME,
    DISK_TYPE_USB,
    DISK_TYPE_RAM
} disk_connection_type_t;

typedef struct {
    uint64_t id;
    uint64_t total_sectors;
    uint32_t sector_size;
    disk_connection_type_t type;
    char model[40];
    uint8_t is_removable;
} disk_info_t;

typedef struct {
    uint64_t id;
    uint64_t parent_disk_id;
    uint64_t start_lba;
    uint64_t size_sectors;
    uint8_t  partition_type;
    uint8_t  bootable;
} partition_info_t;

typedef struct {
    uint64_t framebuffer_addr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
} __attribute__((packed)) sys_video_t;

typedef enum {
	THREAD_PRIO_REALTIME = 0,
	THREAD_PRIO_SYSTEM,
	THREAD_PRIO_NORMAL
} thread_prio_t;

#define THREAD_PRIO_LEVELS 3

typedef enum {
	DRV_STAT_CREATED = 0,
	DRV_STAT_READY
} driver_status_t;

#define AOS_DRIVER_MAGIC 0x44525652
#define DRIVER_NAME_MAX 32

#define DRV_PERM_IO_PORTS          (1 << 0)
#define DRV_PERM_PHYS_MAP          (1 << 1)
#define DRV_PERM_EDIT_SYSTEM_FLAGS (1 << 2)
#define DRV_PERM_GET_SPEC_INFO     (1 << 3)

#define SPEC_INFO_VIDEO 1

typedef struct aos_driver_info_t {
    uint32_t magic;
    uint32_t version;
    driver_type_t type;
    char name[DRIVER_NAME_MAX];
    uint32_t requested_perms;
    uint16_t allowed_ports[8];
} __attribute__((packed)) aos_driver_info_t;

#define AOS_DECLARE_DRIVER(drv_type, perms, ...) \
    __attribute__((section(".driver_info"), used)) \
    const aos_driver_info_t _driver_metadata = { \
        .magic = AOS_DRIVER_MAGIC, \
        .version = 1, \
        .type = drv_type, \
        .requested_perms = perms, \
        .allowed_ports = {__VA_ARGS__} \
    };

#define AOS_HANDLE_SUBTYPE_CHECK(expected_subtype) \
    if (in->subtype != (expected_subtype)) { \
        out->param1 = DRV_ERR_NOCOMM; \
        break; \
    }

apid_t get_driver_pid(driver_type_t type);
apid_t get_driver_pid_name(const char* name);
uint64_t get_driver_pid_sleep_wrapper(void* arg);
driver_type_t dt_from_str(const char* str);
uint64_t get_system_ticks(void);
int get_sysinfo(system_info_t* info);
int set_thread_priority(atid_t target_tid, thread_prio_t priority);
int set_driver_status(apid_t target_pid, driver_status_t status);

void hal_outb(uint16_t port, uint8_t val);
uint8_t hal_inb(uint16_t port);
void hal_outw(uint16_t port, uint16_t val);
uint16_t hal_inw(uint16_t port);
void hal_insw(uint16_t port, void* addr, uint32_t count);
void hal_outsw(uint16_t port, const void* addr, uint32_t count);
void hal_cpu_relax(void);
void hal_memcpy_toio(void* dest, const void* src, size_t bytes);

#ifdef __cplusplus
}
#endif

#endif // AOS_DRIVER_H