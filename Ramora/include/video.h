#ifndef VIDEO_H
#define VIDEO_H

#include <types.h>

#define wid 1920
#define hig 1080

UINT* fbuf = (UINT*)0x04000000;


void 
pixel
(
    int x, 
    int y, 
    UC col
);
void 
drawRec
(
    int x,
    int y, 
    int w, 
    int h, 
    UC col
);

void
backgroundFill
(
    UC col
);

typedef
struct
Rec
{
    int x;
    int y;
    int w;
    int h;
    UC col;
} Rec;

/* we'll leave the rest to panel manager (pmng)*/

#endif