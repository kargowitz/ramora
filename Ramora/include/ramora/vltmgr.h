/*
    hello! this is the first time i am approaching memory mng in this way. ramora will use 
    arrays for RAM because no matter how much you need this memory, C won't 
    overwrite it! 
*/

#ifndef VLTMGR_H
#define VLTMGR_H
#include <types.h>

/* first we need to assign how much memory we'll have*/

#define TOTALMEM   (1024 * 1024 * 1024)  // 1GB?
#define SECTORSIZE (16 * 1024 * 1024)  // 16 MB
#define BLOCKSPERSECTOR 4096
#define BLOCKSIZE  (4 * 1024)  // 4 KB

typedef
struct 
block
{
    UC contents[4096]; // each entry is 1 byte
    int identifier; /*0-4095*/
} 
block;

typedef
struct
sector
{
    UINT* location; /* 0xXXXXXXXXXXX of the sector*/
    int sectorNum; /* will probably use a function to map these to names*/
    int blocksInSector;
    block sectorBlocks[BLOCKSPERSECTOR];
} 
sector;




sector
createNewSector
(
    UINT address
);

void
clearSector
(
    sector* sec
);


#endif
