#ifndef VIDEO_H
#define VIDEO_H

#include <types.h>

#define wid 80
#define hig 25

#define vmem 0xB8000

static USH* vm = (USH*)vmem;
static SIZE vx = 0;
static SIZE vy = 0;
static UC vgc = 0x03; // cyan on black

void
pc
(
    char c
);

void
ps
(
    const char* str
);
void
clr
(
    UC col
);

#endif