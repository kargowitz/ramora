#include <video.h>
#include <types.h>
/*
    this file will provide imp of video.h functions so we can use it more than once
*/

void
pixel
(
    int x,
    int y,
    UC col
)
{
    fbuf[y * wid + x] = col;
}

void
drawRec
(
    int x,
    int y,
    int w,
    int h,
    UC col
)
{
    for
    (
        int hi = 0;
        hi < h;
        hi++
    )
    {
        for
        (
            int wi = 0;
            wi < w;
            wi++
        )
        {
            pixel
            (
                x + wi, y + hi, col
            );
        }
    }
}

void
backgroundFill
(
    UC col
)
{
    drawRec
    (
        0, 
        0, 
        1920, 
        1080, 
        col
    );
}