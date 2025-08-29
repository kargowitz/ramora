#include <video.h>
#include <types.h>

void
pc
(
    char c
)
{
    if
    (
        ++vy >= hig
    )
    {
        vy = hig - 1;
        return;   
    }

    SIZE index = vy * wid + vx;
    vm[index] = (USH)c | (vgc << 8);

    if
    (
        ++vx >= wid
    )
    {
        vx = 0;
        if 
        (
            ++vy >= hig
        )
        {
            vy = hig - 1;
            return;
        }
    }
}

void
ps
(
    const char *str
)
{
    while
    (
        *str
    )
    {
        pc(*str++);
    }
}

void
clr
(
    UC col
)
{
    
    for
    (
        int h = 0;
        h < hig;
        h++
    )
    {
        
       for
       (
            int w = 0;
            w < wid;
            w++
       )
       {
            pc(" ");
       }
        
    }

}
