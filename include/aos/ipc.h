#ifndef _AOS_IPC_H
#define _AOS_IPC_H

#include <aos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MSG_TYPE_NONE = 0,
    MSG_TYPE_KEYBOARD,
    MSG_TYPE_VFS,
    MSG_TYPE_DATA,
    MSG_TYPE_AUTH,
    MSG_TYPE_INPUT,
    MSG_TYPE_VIDEO,
    MSG_TYPE_WND,
    MSG_TYPE_HARDWARE
} msg_type_t;

typedef enum {
    MSG_SUBTYPE_NONE = 0,
    MSG_SUBTYPE_QUERY,
    MSG_SUBTYPE_SEND,
    MSG_SUBTYPE_RESPONSE,
    MSG_SUBTYPE_PING,
    MSG_SUBTYPE_PONG
} msg_subtype_t;

#define HW_EVT_IRQ 1

typedef struct message_t {
    apid_t   sender_pid;
    uint32_t type;
    uint32_t subtype;
    uint64_t id;
    uint64_t param1;
    uint64_t param2;
    uint64_t param3;
    uint8_t  data[64];
} __attribute__((packed, aligned(8))) message_t;

int64_t __ipc_recv(message_t* out_msg);
int64_t __ipc_tryrecv(message_t* out_msg);
int64_t ipc_tryrecv(message_t* out_msg);
int64_t ipc_send(apid_t dest_pid, message_t* msg);
int64_t ipc_reply(message_t* in_msg, message_t* out_msg);
int64_t ipc_requeue(message_t* msg);
uint64_t get_ipc_count(void);
void ipc_sync(void);
void ipc_recv(message_t* out_msg);
void ipc_recv_ex(apid_t pid, msg_type_t type, msg_subtype_t subtype, uint64_t id, message_t* out_msg);
int ipc_tryrecv_ex(apid_t pid, msg_type_t type, msg_subtype_t subtype, uint64_t id, message_t* out_msg);
void ipc_seek(int64_t offset, seek_whence_t whence);
int ipc_get_at(uint64_t index, message_t* out);
int ipc_set_limit(apid_t pid, uint64_t new_limit);

#ifdef __cplusplus
}
#endif

#endif /* _AOS_IPC_H */