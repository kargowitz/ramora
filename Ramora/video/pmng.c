#include <video.h>
#include <types.h>

/*
    panel manager
*/

typedef
struct
Panel 
{
    int x;
    int y;
    int w;
    int h;
    UC col;
} 
Panel;

void
drawPanel
(
    Panel ptd
)
{
    drawRec
    (
        ptd.x,
        ptd.y,
        ptd.w,
        ptd.h,
        ptd.col
    );
}

