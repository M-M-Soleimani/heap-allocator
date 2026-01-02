#include "../include/heap.h"
#include "../include/strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

// Global heap pointer and size
block_meta_t *heap_start = NULL;
static size_t heap_total_size = 0;

// Align size to 8 bytes
static size_t align8(size_t size)
{
    return (size + 7) & ~0x7;
}

/* Split block if it's larger than requested */
void split_block(block_meta_t *block, size_t size)
{
    if (block->size >= size + sizeof(block_meta_t) + 8)
    {
        block_meta_t *new_block = (block_meta_t *)((char *)block + sizeof(block_meta_t) + size);
        new_block->size = block->size - size - sizeof(block_meta_t);
        new_block->free = 1;
        new_block->magic = BLOCK_MAGIC;
        new_block->next = block->next;
        new_block->prev = block;
        if (block->next)
            block->next->prev = new_block;
        block->next = new_block;
        block->size = size;
    }
}

/* Coalesce with next block if free */
void coalesce(block_meta_t *block)
{
    if (block->next && block->next->free)
    {
        block->size += sizeof(block_meta_t) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }
}

/* Platform allocation */
static void *platform_alloc(size_t size)
{
#ifdef _WIN32
    void *ptr = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!ptr)
        errno = ENOMEM;
    return ptr;
#else
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED)
    {
        errno = ENOMEM;
        return NULL;
    }
    return ptr;
#endif
}

static void platform_free(void *ptr, size_t size)
{
    (void)size; // suppress unused parameter warning
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

/* Heap API */
int hinit(size_t heap_size)
{
    heap_size = align8(heap_size);
    void *ptr = platform_alloc(heap_size);
    if (!ptr)
    {
        errno = ENOMEM;
        return HEAP_ERR_OOM;
    }

    heap_start = (block_meta_t *)ptr;
    heap_start->size = heap_size - sizeof(block_meta_t);
    heap_start->free = 1;
    heap_start->next = NULL;
    heap_start->prev = NULL;
    heap_start->magic = BLOCK_MAGIC;

    heap_total_size = heap_size;

    if (!current_strategy)
        current_strategy = first_fit;

    return HEAP_OK;
}

void *halloc(size_t size)
{
    if (!heap_start || size == 0)
    {
        errno = EINVAL;
        return NULL;
    }

    size = align8(size);
    block_meta_t *block = current_strategy(size);
    if (!block)
    {
        errno = ENOMEM;
        return NULL;
    }

    split_block(block, size);
    block->free = 0;
    block->magic = BLOCK_MAGIC;

    return (char *)block + sizeof(block_meta_t);
}

int hfree(void *ptr)
{
    if (!ptr)
    {
        errno = EINVAL;
        return HEAP_ERR_INVALID_PTR;
    }

    block_meta_t *block = (block_meta_t *)((char *)ptr - sizeof(block_meta_t));

    /* Magic check */
    if (block->magic != BLOCK_MAGIC)
    {
        errno = EINVAL;
        return HEAP_ERR_INVALID_PTR;
    }

    // Double free check
    if (block->free)
    {
        errno = EALREADY;
        return HEAP_ERR_DOUBLE_FREE;
    }

    block->free = 1;

    coalesce(block);
    if (block->prev && block->prev->free)
        coalesce(block->prev);

    return HEAP_OK;
}

void hdestroy()
{
    if (!heap_start)
        return;
    platform_free(heap_start, heap_total_size);
    heap_start = NULL;
    heap_total_size = 0;
}

/* Debug */
void heap_dump()
{
    block_meta_t *current = heap_start;
    printf("Heap Dump:\n");
    while (current)
    {
        printf("Block %p | size: %zu | %s\n", (void *)current, current->size,
               current->free ? "free" : "used");
        current = current->next;
    }
}