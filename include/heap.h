#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#define HEAP_ALLOC_FAIL NULL

typedef struct block_meta
{
    size_t size; // block size
    int free;    // 1 if free, 0 if used
    struct block_meta *next;
    struct block_meta *prev;
} block_meta_t;

// Pointer to first block in heap
extern block_meta_t *heap_start;

// Heap API
int hinit(size_t heap_size); // initialize heap
void *halloc(size_t size);   // allocate memory
void hfree(void *ptr);       // free memory
void hdestroy();             // destroy heap
void heap_dump();            // display heap blocks

#endif // HEAP_H