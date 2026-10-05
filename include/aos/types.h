#ifndef _AOS_TYPES_H
#define _AOS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_ZOMBIE
} thread_state_t;

typedef uint32_t apid_t;
typedef uint64_t atid_t;

typedef enum {
    SEEK_SET = 0,
    SEEK_CUR = 1,
    SEEK_END = 2
} seek_whence_t;

#define SYS_RES_OK                    0
#define SYS_RES_INVALID              -1
#define SYS_RES_NO_PERM              -2
#define SYS_RES_ALREADY              -3
#define SYS_RES_DRV_ERR              -4
#define SYS_RES_QUEUE_EMPTY          -5
#define SYS_RES_DSK_ERR              -6
#define SYS_RES_RANGE                -7
#define SYS_RES_NOTFOUND             -8
#define SYS_RES_OOM                  -9
#define SYS_RES_KERNEL_ERR          -99

#define STAT_OK                       0
#define STAT_STACK_SMASHING        -256
#define STAT_NO_ENTRY              -257
#define STAT_OOM                   -258

#define DRV_ERR_OK                    0
#define DRV_ERR_FOUND              -259
#define DRV_ERR_NOCOMM             -258
#define DRV_ERR_NOTFOUND           -257
#define DRV_ERR_UNKNOWN            -256

typedef struct {
    uint64_t uptime;
    uint64_t boot_time;
    uint64_t frequency;
} time_info_t;

typedef enum : uint8_t {
    STARTUP_MAIN = 1,
    STARTUP_DRIVERMAIN
} startup_type_t;

typedef struct {
    startup_type_t type;
    thread_state_t state;
    union {
        struct {
            int argc;
            int envc;
            char** argv;
            char** envp;
        } main;
        struct {
            void* reserved1;
            void* reserved2;
        } driver;
    } data;
} startup_info_t;

typedef struct {
    const char* name;
    uint8_t* data;
    uint64_t size;
    startup_info_t* info;
    uint64_t arg_val;
} spawn_args_t;

typedef struct {
    void*       tcb_self;
    atid_t      tid;
    apid_t      pid;
    int32_t     thread_errno;
    uint32_t    pending_msgs;
    void*       local_heap;
    uint64_t    stack_canary;
    time_info_t startup_time;
} aos_tcb_t;

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__)
#define PEB_VIRT_ADDR 0x00007FFFFE000000ULL
#else
#define PEB_VIRT_ADDR 0x7FFE0000UL
#endif

typedef struct {
    apid_t pid;
    volatile uint64_t pending_msgs; 
    time_info_t startup_time;
    char process_name[32];
} aos_peb_t;

#if defined(__x86_64__) || defined(__i386__)
    #define AOS_GET_TCB() ((aos_tcb_t __seg_fs *)0)
#else
    #error "Unsupported architecture for AOS_GET_TCB()"
#endif
#define AOS_GET_PEB() ((aos_peb_t*)PEB_VIRT_ADDR)

#ifndef UNUSED
    #define UNUSED(x) (void)(x)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AOS_TYPES_H */