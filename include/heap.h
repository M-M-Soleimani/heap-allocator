#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

// Error codes
#define HEAP_OK 0
#define HEAP_ERR_INVALID_PTR 1
#define HEAP_ERR_DOUBLE_FREE 2
#define HEAP_ERR_OOM 3

// Magic number for heap integrity
#define BLOCK_MAGIC 0xDEADBEEF

typedef struct block_meta
{
    size_t size; // block size
    int free;    // 1 if free, 0 if used
    struct block_meta *next;
    struct block_meta *prev;
    uint32_t magic; // integrity check
} block_meta_t;

// Allocation strategies
typedef block_meta_t *(*alloc_strategy_t)(size_t size);
extern alloc_strategy_t current_strategy;

// Pointer to first block in heap
extern block_meta_t *heap_start;

// Heap API
int hinit(size_t heap_size); // returns HEAP_OK / HEAP_ERR_OOM
void *halloc(size_t size);   // returns pointer or NULL
int hfree(void *ptr);        // returns HEAP_OK / error code
void hdestroy(void);
void heap_dump(void);

/* Split and coalesce prototypes */
void split_block(block_meta_t *block, size_t size);
void coalesce(block_meta_t *block);

#endif