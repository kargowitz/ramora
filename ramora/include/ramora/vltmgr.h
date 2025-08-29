/*
    hello! this is the first time i am approaching memory mng in this way. ramora will use 
    arrays for RAM because no matter how much you need this memory, C won't 
    overwrite it! 

    Jules' note: I've refactored the memory manager to use a more traditional
    linked-list approach. This avoids the stack overflow issues from the
    previous design and provides a more flexible foundation for memory
    management.
*/

#ifndef VLTMGR_H
#define VLTMGR_H
#include <types.h>

/*
    A block of memory, which will be part of a linked list of free blocks.
    This struct is just a descriptor, it doesn't hold the memory itself.
*/
typedef struct mem_block {
    SIZE size;              // Size of the free block in bytes
    struct mem_block* next; // Pointer to the next free block
} mem_block_t;

/*
    Represents a region of memory that the allocator can manage.
*/
typedef struct mem_region {
    mem_block_t* free_list; // Head of the free list
} mem_region_t;

/*
    Initializes a memory region for the allocator.
    'start_addr' should be the beginning of the memory you want to manage.
    'size' is the total size of that memory region.
*/
void mem_init(mem_region_t* region, void* start_addr, SIZE size);

/*
    Allocates a block of memory of at least 'size' bytes.
    Returns a pointer to the allocated memory, or NULL if it fails.
*/
void* mem_alloc(mem_region_t* region, SIZE size);

/*
    Frees a previously allocated block of memory.
    'ptr' must be a pointer returned by a previous call to mem_alloc.
*/
void mem_free(mem_region_t* region, void* ptr);


#endif
