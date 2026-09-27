#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <aos/types.h>
#include <aos/utils.h>
#include <aos/vfs.h>

char* to_upper(char* s) {
    char* start = s;
    while (*s != '\0') {
        if (*s >= 'a' && *s <= 'z') {
            *s -= ('a' - 'A');
        }
        s++;
    }
    return start;
}

int is_digit(const char* str) {
    if (str == (void*)0 || *str == '\0') {
        return 0;
    }
    if (*str == '-') {
        str++;
        if (*str == '\0') return 0;
    }
    while (*str != '\0') {
        if (*str < '0' || *str > '9') {
            return 0;
        }
        str++;
    }
    return 1;
}

char* strnchr(const char* s, size_t count, int c) {
    while (count--) {
        if (*s == (char)c) {
            return (char*)s;
        }
        if (*s == '\0') {
            break;
        }
        s++;
    }
    return (void*)0;
}

static bool is_clean_tail(const char *endptr) {
    return (*endptr == '\0' || (*endptr == '\n' && *(endptr + 1) == '\0'));
}

int kstrtoull(const char *s, int base, unsigned long long *res) {
    char *endptr;
    
    const char *check_sign = s;
    while (isspace(*check_sign)) check_sign++;
    
    if (*check_sign == '-') {
        return SYS_RES_INVALID; 
    }

    unsigned long long val = strtoull(s, &endptr, base);

    if (endptr == s || !is_clean_tail(endptr)) {
        return SYS_RES_INVALID;
    }

    if (val == ULLONG_MAX) {
        return SYS_RES_RANGE;
    }

    *res = val;
    return SYS_RES_OK;
}

int kstrtoll(const char *s, int base, long long *res) {
    char *endptr;
    
    long long val = strtoll(s, &endptr, base);

    if (endptr == s || !is_clean_tail(endptr)) {
        return SYS_RES_INVALID;
    }

    if (val == LLONG_MAX || val == LLONG_MIN) {
        return SYS_RES_RANGE;
    }

    *res = val;
    return SYS_RES_OK;
}

int kstrtoint(const char *s, int base, int *res) {
    long long val;
    
    int err = kstrtoll(s, base, &val);
    
    if (err != SYS_RES_OK) {
        return err;
    }

    if (val < INT_MIN || val > INT_MAX) {
        return SYS_RES_RANGE;
    }

    *res = (int)val;
    return SYS_RES_OK;
}

int kstrtobool(const char *s, bool *res) {
    while (isspace(*s)) s++;

    switch (*s) {
        case '1':
            *res = true;
            return SYS_RES_OK;
        case '0':
            *res = false;
            return SYS_RES_OK;
        case 'y': case 'Y':
            *res = true;
            return SYS_RES_OK;
        case 'n': case 'N':
            *res = false;
            return SYS_RES_OK;
        case 't': case 'T':
            if ((s[1] == 'r' || s[1] == 'R') && 
                (s[2] == 'u' || s[2] == 'U') && 
                (s[3] == 'e' || s[3] == 'E')) {
                *res = true; return SYS_RES_OK;
            }
            break;
        case 'f': case 'F':
            if ((s[1] == 'a' || s[1] == 'A') && 
                (s[2] == 'l' || s[2] == 'L') && 
                (s[3] == 's' || s[3] == 'S') && 
                (s[4] == 'e' || s[4] == 'E')) {
                *res = false; return SYS_RES_OK;
            }
            break;
        case 'o': case 'O':
            if (s[1] == 'n' || s[1] == 'N') { 
                *res = true; return SYS_RES_OK; 
            }
            if ((s[1] == 'f' || s[1] == 'F') && 
                (s[2] == 'f' || s[2] == 'F')) { 
                *res = false; return SYS_RES_OK; 
            }
            break;
    }

    return SYS_RES_INVALID;
}

void free_image(image_t* img) {
    if (!img) return;
    if (img->pixels) free(img->pixels);
    free(img);
}

image_t* load_bmp(const char* path) {
    if (!path) return NULL;

    int fd = vfs_open(path, VFS_FREAD);
    if (fd < 0) return NULL;

    vfs_stat_info_t stat;
    if (vfs_stat(fd, &stat) != 0 || stat.size_bytes < 54) {
        vfs_close(fd);
        return NULL;
    }

    uint8_t* file_data = (uint8_t*)malloc(stat.size_bytes);
    if (!file_data) {
        vfs_close(fd);
        return NULL;
    }

    uint64_t total_read = 0;
    while (total_read < stat.size_bytes) {
        int r = vfs_read(fd, file_data + total_read, stat.size_bytes - total_read);
        if (r <= 0) break;
        total_read += r;
    }
    vfs_close(fd);

    if (total_read != stat.size_bytes) {
        free(file_data);
        return NULL;
    }

    if (file_data[0] != 0x42 || file_data[1] != 0x4D) {
        free(file_data);
        return NULL;
    }

    uint32_t data_offset = *(uint32_t*)&file_data[10];
    int32_t  width       = *(int32_t*)&file_data[18];
    int32_t  height      = *(int32_t*)&file_data[22];
    uint16_t bpp         = *(uint16_t*)&file_data[28];

    if (bpp != 24 && bpp != 32) {
        free(file_data);
        return NULL;
    }

    int is_bottom_up = 1;
    if (height < 0) {
        is_bottom_up = 0;
        height = -height;
    }

    image_t* img = (image_t*)malloc(sizeof(image_t));
    if (!img) {
        free(file_data);
        return NULL;
    }
    
    img->width = width;
    img->height = height;
    img->pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    
    if (!img->pixels) {
        free(img);
        free(file_data);
        return NULL;
    }

    uint8_t* src_pixels = file_data + data_offset;
    int row_padding = (4 - ((width * (bpp / 8)) % 4)) % 4;

    for (int y = 0; y < height; y++) {
        int dest_y = is_bottom_up ? (height - 1 - y) : y;
        uint32_t* dest_row = img->pixels + (dest_y * width);
        
        uint8_t* src_row = src_pixels + y * (width * (bpp / 8) + row_padding);

        for (int x = 0; x < width; x++) {
            uint8_t b = src_row[x * (bpp / 8) + 0];
            uint8_t g = src_row[x * (bpp / 8) + 1];
            uint8_t r = src_row[x * (bpp / 8) + 2];
            uint8_t a = (bpp == 32) ? src_row[x * (bpp / 8) + 3] : 255;

            dest_row[x] = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }
    }

    free(file_data);
    return img;
}