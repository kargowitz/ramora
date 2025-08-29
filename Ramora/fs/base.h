#ifndef BASE_H
#define BASE_H

#include <types.h>

/* define what a file and inode are*/
// so what actually are they?
/*
    inode: metadata for a file, e.g file size, file type without actually storing the
    contents. essentially pointers with info to the file.
    file: place to store contents under 1 name
    (custom definitions)
*/

typedef
enum
{
    FTREGULAR,
    FTDIR
} 
FILETYPE;

typedef
struct
inode
{
    int id; // special way of finding out what we're referring to.
    FILETYPE type; // stores the type of file
    UC size;
    int blcount;
    UINT blc[67];
};


#endif