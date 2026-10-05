#ifndef _AOS_SYSINFO_H
#define _AOS_SYSINFO_H

#include <aos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t uptime;
    uint64_t fs_base;
    uint64_t gs_base;
    uint64_t kernel_gs_base;
    uint32_t flags;
    uint16_t cpu_flags;
} system_info_t;

#ifndef AOSKERNEL
int get_sysinfo(system_info_t* info);
uint64_t get_system_ticks(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AOS_SYSINFO_H */