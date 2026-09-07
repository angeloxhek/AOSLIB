#ifndef _AOS_SYSINFO_H
#define _AOS_SYSINFO_H

#include <aos/types.h>
#include <aos/process.h>
#include <aos/driver.h>

#ifdef __cplusplus
extern "C" {
#endif

int get_sysinfo(system_info_t* info);
uint64_t get_system_ticks(void);

#ifdef __cplusplus
}
#endif

#endif /* _AOS_SYSINFO_H */