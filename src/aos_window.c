#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <aos/types.h>
#include <aos/driver.h>
#include <aos/ipc.h>
#include <aos/syscalls.h>
#include <aos/window.h>
#include <agfx_ui.h>

static apid_t wnd_driver_pid = 0;

#define ensure_wnd_init() { if (wnd_driver_pid == 0) wnd_driver_pid = get_driver_pid(DT_WND); }

static int wnd_rpc_call(message_t* req, message_t* resp_out) {
    ensure_wnd_init();

    req->type = MSG_TYPE_WND;

	int64_t id;
	do {
		id = ipc_send(wnd_driver_pid, req);
	} while (id == SYS_RES_NOTFOUND);
	
	if (id < 0) return id;

    ipc_recv_ex(
        wnd_driver_pid,
        MSG_TYPE_WND,
        MSG_SUBTYPE_NONE,
		(uint64_t)id,
        resp_out
    );

    return (int)resp_out->param1;
}

window_t* window_create(int x, int y, int w, int h, uint32_t flags) {
    ensure_wnd_init();

    window_t* win = (window_t*)malloc(sizeof(window_t));
    if (!win) return 0;

    win->w = w;
    win->h = h;

    uint64_t frame_size = (uint64_t)w * h * 4;
    win->shm_id = shm_alloc(frame_size * 2, (void**)&win->buffer);
    if (!win->shm_id) { free(win); return 0; }

    shm_allow(win->shm_id, wnd_driver_pid);

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    memset(&resp, 0, sizeof(message_t));

    req.type = MSG_TYPE_WND;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = WND_CMD_CREATE;
    
    wnd_create_req_t* _req = (wnd_create_req_t*)req.data;
    _req->x = x;
    _req->y = y;
    _req->width = w;
    _req->height = h;
    _req->flags = flags;
    _req->shm_id = win->shm_id;
	
	int res = wnd_rpc_call(&req, &resp);

    if (res == WND_ERR_OK) {
        win->win_id = resp.param2;
        return win;
    }

    shm_free(win->shm_id);
    free(win);
    return 0;
}

void window_flush(window_t* win) {
    if (!win) return;
    ensure_wnd_init();

    uint64_t frame_size = (uint64_t)win->w * win->h * 4;
    uint8_t* back_buffer = (uint8_t*)win->buffer;
    uint8_t* front_buffer = back_buffer + frame_size;
    
    hal_memcpy_toio(front_buffer, back_buffer, frame_size);

    message_t req;
    memset(&req, 0, sizeof(message_t));
    req.type = MSG_TYPE_WND;
    req.subtype = MSG_SUBTYPE_SEND;
    req.param1 = WND_CMD_FLUSH;
    req.param2 = win->win_id;
    ipc_send(wnd_driver_pid, &req);
}

int get_screen_info(screen_info_t* info) {
	if (!info) return -1;
    ensure_wnd_init();

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    req.type = MSG_TYPE_WND;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = WND_CMD_GET_SCREEN_INFO;

	int res = wnd_rpc_call(&req, &resp);

    if (res == WND_ERR_OK) {
        info->width = (uint16_t)(resp.param2 >> 16);
        info->height = (uint16_t)(resp.param2 & 0xFFFF);
    }
    return res;
}

void window_destroy(window_t* win) {
    if (!win) return;
    ensure_wnd_init();
    if (wnd_driver_pid == 0) return;

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    memset(&resp, 0, sizeof(message_t));

    req.type = MSG_TYPE_WND;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = WND_CMD_DESTROY;
    req.param2 = win->win_id;

    wnd_rpc_call(&req, &resp);

    shm_free(win->shm_id);
    free(win);
}

int get_system_theme(void* out_theme) {
    if (!out_theme) return -1;
    ensure_wnd_init();

    void* shm_vaddr = 0;
    uint64_t shm_id = shm_alloc(sizeof(agfx_ui_theme_t), &shm_vaddr);
    if (!shm_id) return -1;

    shm_allow(shm_id, wnd_driver_pid);

    message_t req, resp;
    memset(&req, 0, sizeof(message_t));
    memset(&resp, 0, sizeof(message_t));

    req.type = MSG_TYPE_WND;
    req.subtype = MSG_SUBTYPE_QUERY;
    req.param1 = WND_CMD_GET_THEME;
    *(uint64_t*)(req.data) = shm_id;

    int res = wnd_rpc_call(&req, &resp);

    if (res == WND_ERR_OK) {
        memcpy(out_theme, shm_vaddr, sizeof(agfx_ui_theme_t));
    }

    shm_free(shm_id);
    return res;
}

int agfx_get_system_theme_hook(agfx_ui_theme_t* out_theme) {
    return get_system_theme((void*)out_theme);
}