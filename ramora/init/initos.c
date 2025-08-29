#include <video.h>
#include <terminal/io.h>
#include <fs/base.h>

extern void fsinit();

void loados()
{
    fsinit();
    clr(0x03);
    ps("Welcome to Ramora!\n");

    char inbuf[32];
    while (1)
    {
        read(inbuf, 32);
        parse(inbuf);
    }

}