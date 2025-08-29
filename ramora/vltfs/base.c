#include <fs/base.h>
#include <fs/ata/ata.h>
#include <types.h>

// Filesystem layout constants
#define NINODES 200
#define FSSIZE 10000 // Total blocks in the filesystem

// A static buffer for block-sized I/O
static char fs_buffer[BLOCK_SIZE];

// The superblock of the mounted filesystem
struct superblock sb;

// Helper function to read a full block from the disk
static void read_block(UINT block_no, void* buf) {
    // A block is 4096 bytes, a sector is 512. So, 8 sectors per block.
    ata_read_sectors(block_no * (BLOCK_SIZE / ATA_SECTOR_SIZE), (BLOCK_SIZE / ATA_SECTOR_SIZE), buf);
}

// Helper function to write a full block to the disk
static void write_block(UINT block_no, const void* buf) {
    ata_write_sectors(block_no * (BLOCK_SIZE / ATA_SECTOR_SIZE), (BLOCK_SIZE / ATA_SECTOR_SIZE), buf);
}

// Format the filesystem on the disk
void fs_format() {
    // 1. Create the superblock in memory
    sb.size = FSSIZE;
    sb.ninodes = NINODES;
    sb.inode_start = 2; // Inodes start at block 2 (block 0 is boot, 1 is sb)

    // Calculate how many blocks the inode table will need
    UINT inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
    UINT inode_blocks = (NINODES + inodes_per_block - 1) / inodes_per_block;

    sb.bmap_start = sb.inode_start + inode_blocks;

    // The rest of the disk is data blocks.
    sb.nblocks = FSSIZE - sb.bmap_start - 1; // -1 for the bitmap block itself

    // 2. Write the superblock to block 1
    // We zero the buffer first to ensure the block is clean.
    for (int i = 0; i < BLOCK_SIZE; i++) fs_buffer[i] = 0;
    // Copy the superblock structure to the beginning of the buffer.
    *(struct superblock*)fs_buffer = sb;
    write_block(1, fs_buffer);

    // 3. Zero out the inode table and the free block bitmap
    for (int i = 0; i < BLOCK_SIZE; i++) fs_buffer[i] = 0;
    for (UINT i = 0; i < inode_blocks; i++) {
        write_block(sb.inode_start + i, fs_buffer);
    }
    write_block(sb.bmap_start, fs_buffer);
}

// Mount the filesystem from the disk
void fs_mount() {
    // 1. Initialize the underlying ATA driver
    ata_init();

    // 2. Read the superblock from disk (block 1) into our buffer
    read_block(1, fs_buffer);
    // Copy the data from the buffer into our global superblock variable
    sb = *(struct superblock*)fs_buffer;

    // The filesystem is now ready to be used.
    // A real implementation might check for a magic number to verify it's a valid fs.
}

// --- Block and Inode Management ---

// Allocate a free data block from the bitmap.
// Returns the block number of the allocated block, or 0 on failure.
UINT balloc() {
    read_block(sb.bmap_start, fs_buffer);

    for (UINT b = 0; b < sb.nblocks; b++) {
        int byte_idx = b / 8;
        int bit_idx = b % 8;

        if ((fs_buffer[byte_idx] & (1 << bit_idx)) == 0) { // If the bit is 0, the block is free
            fs_buffer[byte_idx] |= (1 << bit_idx); // Mark block as used
            write_block(sb.bmap_start, fs_buffer);

            // Return the absolute block number on disk
            return sb.bmap_start + 1 + b;
        }
    }
    // No free blocks found
    return 0;
}

// Free a data block.
void bfree(UINT block_no) {
    read_block(sb.bmap_start, fs_buffer);

    // Calculate the block's bit position in the bitmap
    UINT b = block_no - (sb.bmap_start + 1);
    int byte_idx = b / 8;
    int bit_idx = b % 8;

    fs_buffer[byte_idx] &= ~(1 << bit_idx); // Mark block as free by clearing the bit
    write_block(sb.bmap_start, fs_buffer);
}

// Allocate a free inode from the inode table.
// Returns the inode number, or 0 on failure.
UINT ialloc() {
    UINT inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
    UINT inode_blocks = (sb.ninodes + inodes_per_block - 1) / inodes_per_block;

    for (UINT i = 0; i < inode_blocks; i++) {
        read_block(sb.inode_start + i, fs_buffer);
        struct inode* inodes = (struct inode*)fs_buffer;

        for (UINT j = 0; j < inodes_per_block; j++) {
            UINT inum = i * inodes_per_block + j;
            if (inum >= sb.ninodes) break; // Don't go past the last inode

            if (inodes[j].type == FT_UNUSED) {
                // Found a free inode. The caller will initialize it.
                return inum;
            }
        }
    }

    // No free inodes found

    return 0;
}

// --- Inode and File Operations ---

// Read an inode from disk.
static void iget(UINT inum, struct inode* ip) {
    UINT inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
    UINT block_no = sb.inode_start + (inum / inodes_per_block);
    UINT inode_offset = inum % inodes_per_block;

    read_block(block_no, fs_buffer);
    *ip = ((struct inode*)fs_buffer)[inode_offset];
}

// Write an inode to disk.
static void iput(UINT inum, struct inode* ip) {
    UINT inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
    UINT block_no = sb.inode_start + (inum / inodes_per_block);
    UINT inode_offset = inum % inodes_per_block;

    read_block(block_no, fs_buffer); // Read the block first to not clobber other inodes
    ((struct inode*)fs_buffer)[inode_offset] = *ip;
    write_block(block_no, fs_buffer);
}

// Map a logical block number within a file to a physical block number on disk.
// Allocates new blocks if necessary.
static UINT bmap(struct inode* ip, UINT bn) {
    UINT addr;
    static char indirect_buffer[BLOCK_SIZE];

    if (bn < NDIRECT) {
        // It's a direct block
        if ((addr = ip->addrs[bn]) == 0) {
            addr = balloc();
            if(addr == 0) return 0; // Out of space
            ip->addrs[bn] = addr;
        }
        return addr;
    }

    bn -= NDIRECT;

    if (bn < NINDIRECT) {
        // It's a single indirect block
        if ((addr = ip->addrs[NDIRECT]) == 0) {
            // Allocate the indirect block itself
            addr = balloc();
            if(addr == 0) return 0; // Out of space
            ip->addrs[NDIRECT] = addr;
        }

        read_block(addr, indirect_buffer);
        UINT* indirect_addrs = (UINT*)indirect_buffer;

        if ((addr = indirect_addrs[bn]) == 0) {
            addr = balloc();
            if(addr == 0) return 0; // Out of space
            indirect_addrs[bn] = addr;
            write_block(ip->addrs[NDIRECT], indirect_buffer); // Write back the modified indirect block
        }
        return addr;
    }

    // Double indirect blocks not supported in this simple fs
    return 0;
}

// Read data from an inode.
// Returns number of bytes read, or -1 on error.
int readi(UINT inum, char* buf, UINT offset, UINT n) {
    struct inode ip;
    iget(inum, &ip);

    if (offset > ip.size || offset + n < offset) {
        return -1; // Invalid offset
    }
    if (offset + n > ip.size) {
        n = ip.size - offset; // Read only up to the end of the file
    }

    UINT tot, m;
    UINT bn;
    for (tot = 0; tot < n; tot += m, offset += m, buf += m) {
        bn = bmap(&ip, offset / BLOCK_SIZE);
        if (bn == 0) {
            break; // Should not happen for a read within file size
        }
        read_block(bn, fs_buffer);

        m = n - tot;
        UINT off_in_block = offset % BLOCK_SIZE;
        if (m > BLOCK_SIZE - off_in_block) {
            m = BLOCK_SIZE - off_in_block;
        }

        for(UINT i = 0; i < m; i++) {
            buf[i] = fs_buffer[off_in_block + i];
        }
    }

    return tot;
}

// Write data to an inode.
// Returns number of bytes written, or -1 on error.
int writei(UINT inum, const char* buf, UINT offset, UINT n) {
    struct inode ip;
    iget(inum, &ip);

    UINT tot, m;
    UINT bn;
    for (tot = 0; tot < n; tot += m, offset += m, buf += m) {
        bn = bmap(&ip, offset / BLOCK_SIZE);
        if (bn == 0) {
            break; // Error from bmap (e.g., disk full)
        }

        UINT off_in_block = offset % BLOCK_SIZE;
        m = n - tot;
        if (m > BLOCK_SIZE - off_in_block) {
            m = BLOCK_SIZE - off_in_block;
        }

        // A read-modify-write is needed for writes that don't align to block boundaries.
        read_block(bn, fs_buffer);
        for(UINT i = 0; i < m; i++) {
            fs_buffer[off_in_block + i] = buf[i];
        }
        write_block(bn, fs_buffer);
    }

    // Only update inode if we actually wrote something.
    if (tot > 0) {
        // If the write extended the file, update the size.
        if (offset > ip.size) {
            ip.size = offset;
        }
        // Write the inode back to disk to persist size and new block allocations.
        iput(inum, &ip);
    }

    return tot;
}

struct file {
    UINT inum;     // inode number
    UINT offset;   // current position in the file
};

struct file fs_open(UINT inum) {
    struct file f = {0, 0};
    if (inum == 0) return f;   // invalid inode
    f.inum = inum;
    f.offset = 0;
    return f;
}

int fs_read(struct file *f, char *buf, UINT n) {
    if (f->inum == 0) return -1;
    int r = readi(f->inum, buf, f->offset, n);
    if (r > 0) f->offset += r; // advance cursor
    return r;
}

int fs_write(struct file *f, const char *buf, UINT n) {
    if (f->inum == 0) return -1;
    int r = writei(f->inum, buf, f->offset, n);
    if (r > 0) f->offset += r; // advance cursor
    return r;
}

struct dirent
{
    char name[32];
    UINT inum;
};



int checkfs() // TODO
{
}