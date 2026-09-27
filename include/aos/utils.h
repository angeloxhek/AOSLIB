#ifndef _AOS_UTILS_H
#define _AOS_UTILS_H

#include <aos/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int width;
    int height;
    uint32_t* pixels;
} image_t;

char* strnchr(const char* s, uint64_t count, int32_t c);
int32_t is_digit(const char* str);
char* to_upper(char* s);

int kstrtoull(const char *s, int base, unsigned long long *res);
int kstrtoll(const char *s, int base, long long *res);
int kstrtoint(const char *s, int base, int *res);
int kstrtobool(const char *s, bool *res);

image_t* load_bmp(const char* path);
void free_image(image_t* img);

#ifdef __cplusplus
}
#endif

#endif /* _AOS_UTILS_H */