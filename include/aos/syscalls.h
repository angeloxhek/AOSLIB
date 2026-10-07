#ifndef _AOS_SYSCALLS_H
#define _AOS_SYSCALLS_H

#include <aos/types.h>
#include <aos/auth.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_EXIT                       1
#define SYS_IPC_SEND                   2
#define SYS_IPC_TRYRECV                3
#define SYS_IPC_RECV                   4
#define SYS_REGISTER_DRIVER            5
#define SYS_GET_DRIVER_PID             6
#define SYS_GET_DRIVER_PID_BY_NAME     7
#define SYS_GET_SYSTEM_INFO            8
#define SYS_SBRK                       9
#define SYS_BLOCK_READ                10
#define SYS_BLOCK_WRITE               11
#define SYS_GET_DISK_COUNT            12
#define SYS_GET_DISK_INFO             13
#define SYS_GET_PARTITION_COUNT       14
#define SYS_GET_PARTITION_INFO        15
#define SYS_YIELD                     16
#define SYS_PRINT                     17
#define SYS_SHM_ALLOC                 18
#define SYS_SHM_ALLOW                 19
#define SYS_SHM_MAP                   20
#define SYS_SHM_FREE                  21
#define SYS_GET_PID_LIST              22
#define SYS_GET_PROC_INFO             23
#define SYS_GET_TID_LIST              24
#define SYS_GET_THREAD_INFO           25
#define SYS_GET_TIME_INFO             26
#define SYS_SPAWN                     27
#define SYS_FORK                      28
#define SYS_EXEC                      29
#define SYS_SET_IPC_LIMIT             30
#define SYS_SHM_GET_SIZE              31
#define SYS_SLEEP                     32
#define SYS_EDIT_SYSTEM_FLAGS         33
#define SYS_MAP_PHYS                  34
#define SYS_GET_SPEC_INFO             35
#define SYS_SET_PROCESS_AUTH          36
#define SYS_SET_THREAD_STATE          37
#define SYS_SET_THREAD_PRIORITY       38
#define SYS_SET_DRIVER_STATUS         39
#define SYS_IPC_REPLY                 40
#define SYS_IPC_REQUEUE               41

#ifndef AOSKERNEL
int64_t syscall(uint64_t nr, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);
void* syscall_sbrk(int64_t increment);
void sysprint(const char* str);
void thread_yield(void);
void syssleep(uint64_t ms);

int sysspawn(const char* path, startup_info_t* info, uint64_t arg2, apid_t* respid);
int sysspawnex(spawn_args_t* args, apid_t* respid);
uint32_t sysfork(void);
int sysexec(const char* path, startup_info_t* info, uint64_t arg2);
int sysexecex(spawn_args_t* args);

int sysedit_sys_flags(uint32_t flags);
int sysmap_phys(uint64_t phys_addr, uint64_t size_bytes, uint64_t* out_vaddr);
int sysset_process_auth(apid_t target_pid, auth_id_t user);

uint64_t shm_alloc(uint64_t size_bytes, void** out_vaddr);
int shm_allow(uint64_t shm_id, apid_t target_pid);
void* shm_map(uint64_t shm_id);
int shm_free(uint64_t shm_id);
uint64_t shm_get_size(uint64_t shm_id);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AOS_SYSCALLS_H */