#ifndef X86_H
#define X86_H

#include <types.h>

// Read a byte from a port
static inline UC inb(USH port) {
    UC data;
    asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    return data;
}

// Write a byte to a port
static inline void outb(USH port, UC data) {
    asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

// Read a word (2 bytes) from a port
static inline USH inw(USH port) {
    USH data;
    asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    return data;
}

// Write a word (2 bytes) to a port
static inline void outw(USH port, USH data) {
    asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

// Read 'count' long words (4 bytes) from a port
static inline void insl(int port, void *addr, int cnt) {
    asm volatile("cld; rep insl"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
}

// Write 'count' long words (4 bytes) to a port
static inline void outsl(int port, const void *addr, int cnt) {
    asm volatile("cld; rep outsl"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
}

#endif // X86_H
