#ifndef AOS_PROCESS_H
#define AOS_PROCESS_H

#include "types.h"
#include "auth.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    apid_t   pid;
    char     name[32];
    uint64_t heap_limit;
    uint64_t threads_count;
    auth_id_t user;
    atid_t main_thread;
} proc_info_user_t;

typedef struct {
    atid_t tid;
    apid_t parent_pid;
    thread_state_t  state;
    int      waiting_for_msg; 
    uint64_t wake_up_time;
} thread_info_user_t;

#ifndef AOSKERNEL
int get_proc_info(apid_t pid, proc_info_user_t* out_info);
int get_thread_info(atid_t tid, thread_info_user_t* out_info);
int get_pid_list(apid_t* buff, uint64_t* count);
int get_tid_list(apid_t pid, atid_t* buff, uint64_t* count);
int get_time_info(time_info_t* info);

int sleep_while_zero(uint64_t (*func)(void*), void* arg, uint64_t timeout_ms, uint64_t* out_result);
#endif

#ifdef __cplusplus
}
#endif
#endif // AOS_PROCESS_H