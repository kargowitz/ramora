#include <fs/base.h>
#include <video.h>

void fsinit()
{

    fs_format();

    fs_mount();

    UINT root = ialloc();
}
