#ifndef VFS_DRIVER_H
#define VFS_DRIVER_H

#include <stdint.h>

typedef void* disk_instance_t;
typedef struct disk_driver disk_driver_t;

typedef struct {
    disk_driver_t* driver;
    disk_instance_t disk_inst;
    
    uint32_t disk_id;
    uint64_t partition_offset_lba;
    uint64_t size_sectors;
    uint32_t sector_size;
} block_dev_t;

typedef void* fs_file_handle_t;
typedef void* fs_context_t; 

typedef void* fs_instance_t;

typedef struct {
    char name[256];
    uint64_t size;
    uint8_t type;
} fs_dirent_t;

typedef struct {
    uint64_t inode_id;
    uint64_t size_bytes;
    uint32_t attributes;
    uint32_t uid;
    uint32_t gid;
    uint16_t mode;
} fs_stat_t;

typedef struct {
    const char name[64];
    fs_instance_t (*mount)(block_dev_t* dev, fs_context_t context);
    void (*umount)(fs_instance_t fs);
    fs_file_handle_t (*open)(fs_instance_t fs, const char* path, uint32_t flags);
    fs_file_handle_t (*openat)(fs_instance_t fs, fs_file_handle_t dir_handle, const char* path, uint32_t flags);
    int (*read)(fs_instance_t fs, fs_file_handle_t file, void* buf, uint64_t size, uint64_t offset);
    int (*write)(fs_instance_t fs, fs_file_handle_t file, const void* buf, uint64_t size, uint64_t offset);
    void (*close)(fs_instance_t fs, fs_file_handle_t file);
    int (*readdir)(fs_instance_t fs, fs_file_handle_t dir, uint64_t* offset, fs_dirent_t* out_array, int max_entries);
    int (*stat)(fs_instance_t fs, fs_file_handle_t file, fs_stat_t* out_info); 
    void (*get_label)(fs_instance_t fs, char* out_label);
    int (*unlinkat)(fs_instance_t fs, fs_file_handle_t dir_handle, const char* name, uint64_t* out_deleted_inode);
} fs_driver_t;

#endif /* FS_INTERFACE_H */