#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <aos/driver.h>

void hal_outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t hal_inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

uint16_t hal_inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ( "inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

void hal_outw(uint16_t port, uint16_t val) {
    __asm__ volatile ( "outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

uint32_t hal_inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ( "inl %w1, %k0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

void hal_outl(uint16_t port, uint32_t val) {
    __asm__ volatile ( "outl %k0, %w1" : : "a"(val), "Nd"(port) : "memory");
}


void hal_insw(uint16_t port, void* addr, uint32_t count) {
    __asm__ volatile ( "cld; rep insw" : "+D" (addr), "+c" (count) : "d" (port) : "memory" );
}

void hal_outsw(uint16_t port, const void* addr, uint32_t count) {
    __asm__ volatile ( "cld; rep outsw" : "+S" (addr), "+c" (count) : "d" (port) : "memory" );
}

void hal_cpu_relax(void) {
    asm volatile("pause" ::: "memory");
}

__attribute__((target("sse,sse2")))
void hal_memcpy_toio_aligned(void* dest, const void* src, size_t bytes) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    while (bytes >= 64) {
        __asm__ volatile (
            "movups 0(%1), %%xmm0\n"
            "movups 16(%1), %%xmm1\n"
            "movups 32(%1), %%xmm2\n"
            "movups 48(%1), %%xmm3\n"
            
            "movntps %%xmm0, 0(%0)\n"
            "movntps %%xmm1, 16(%0)\n"
            "movntps %%xmm2, 32(%0)\n"
            "movntps %%xmm3, 48(%0)\n"
            :
            : "r"(d), "r"(s)
            : "xmm0", "xmm1", "xmm2", "xmm3", "memory"
        );
        d += 64;
        s += 64;
        bytes -= 64;
    }

    while (bytes >= 16) {
        __asm__ volatile (
            "movups (%1), %%xmm0\n"
            "movntps %%xmm0, (%0)\n"
            : "+r"(d), "+r"(s) : : "xmm0", "memory"
        );
        d += 16;
        s += 16;
        bytes -= 16;
    }

    while (bytes--) *d++ = *s++;

    __asm__ volatile ("sfence" ::: "memory");
}

__attribute__((target("sse,sse2")))
void hal_memcpy_toio(void* dest, const void* src, size_t bytes) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    while (bytes >= 64) {
        __asm__ volatile (
            "movups 0(%1),  %%xmm0\n"
            "movups 16(%1), %%xmm1\n"
            "movups 32(%1), %%xmm2\n"
            "movups 48(%1), %%xmm3\n"
            
            "movups %%xmm0, 0(%0)\n"
            "movups %%xmm1, 16(%0)\n"
            "movups %%xmm2, 32(%0)\n"
            "movups %%xmm3, 48(%0)\n"
            :
            : "r"(d), "r"(s)
            : "xmm0", "xmm1", "xmm2", "xmm3", "memory"
        );
        d += 64;
        s += 64;
        bytes -= 64;
    }

    while (bytes >= 16) {
        __asm__ volatile (
            "movups (%1), %%xmm0\n"
            "movups %%xmm0, (%0)\n"
            : 
            : "r"(d), "r"(s) 
            : "xmm0", "memory"
        );
        d += 16;
        s += 16;
        bytes -= 16;
    }

    while (bytes--) {
        *d++ = *s++;
    }
}