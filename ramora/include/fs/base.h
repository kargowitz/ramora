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

    Jules' note: I've redesigned these structures to be more robust and scalable,
    following a more traditional Unix-like filesystem design.
*/

#define BLOCK_SIZE 4096 // Assuming a block size of 4KB

// Filesystem Superblock
// Contains metadata about the entire filesystem.
struct superblock {
    UINT size;          // Size of the entire filesystem in blocks
    UINT nblocks;       // Number of data blocks
    UINT ninodes;       // Number of inodes
    UINT inode_start;   // Block number of the first inode block
    UINT bmap_start;    // Block number of the free block bitmap
};

// File types
typedef enum {
    FT_UNUSED, // Inode is not used
    FT_REGULAR,
    FT_DIR
} FILETYPE;

// Inode structure (on-disk)
#define NDIRECT 12
#define NINDIRECT (BLOCK_SIZE / sizeof(UINT))
#define MAXFILE (NDIRECT + NINDIRECT)

struct inode {
    FILETYPE type;      // File type
    short nlink;        // Number of links to this inode
    SIZE size;          // Size of file in bytes
    UINT addrs[NDIRECT + 1]; // Data block addresses: 12 direct, 1 single indirect
};

// Directory entry
#define DIRSIZ 14

struct dirent {
    USH inum;        // Inode number
    char name[DIRSIZ];  // Filename
};

static char fs_buffer[BLOCK_SIZE];

// The superblock of the mounted filesystem
struct superblock sb;

// Helper function to read a full block from the disk
static void read_block(UINT block_no, void* buf);
static void write_block(UINT block_no, const void* buf);
void fs_format();
void fs_mount();
UINT balloc();
void bfree(UINT block_no);
UINT ialloc();
static void iget(UINT inum, struct inode* ip);
static void iput(UINT inum, struct inode* ip);
static UINT bmap(struct inode* ip, UINT bn);
int readi(UINT inum, char* buf, UINT offset, UINT n);
int writei(UINT inum, const char* buf, UINT offset, UINT n);

#endif