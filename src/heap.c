#include "../include/heap.h"
#include "../include/strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Split block if it's larger than requested
static void split_block(block_meta_t *block, size_t size)
{
    if (block->size >= size + sizeof(block_meta_t) + 8)
    {
        block_meta_t *new_block = (block_meta_t *)((char *)block + sizeof(block_meta_t) + size);
        new_block->size = block->size - size - sizeof(block_meta_t);
        new_block->free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        if (block->next)
            block->next->prev = new_block;
        block->next = new_block;
        block->size = size;
    }
}

// Coalesce with next block if free
static void coalesce(block_meta_t *block)
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
    return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED)
        return NULL;
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
        return -1;

    heap_start = (block_meta_t *)ptr;
    heap_start->size = heap_size - sizeof(block_meta_t);
    heap_start->free = 1;
    heap_start->next = NULL;
    heap_start->prev = NULL;

    heap_total_size = heap_size;

    // Default strategy if not set
    if (!current_strategy)
        current_strategy = first_fit;

    return 0;
}

void *halloc(size_t size)
{
    if (!heap_start || size == 0)
        return HEAP_ALLOC_FAIL;

    size = align8(size);
    block_meta_t *block = current_strategy(size); // select block
    if (!block)
        return HEAP_ALLOC_FAIL;

    split_block(block, size);
    block->free = 0;
    return (char *)block + sizeof(block_meta_t);
}

void hfree(void *ptr)
{
    if (!ptr)
        return;

    block_meta_t *block = (block_meta_t *)((char *)ptr - sizeof(block_meta_t));
    block->free = 1;

    coalesce(block); // merge with next
    if (block->prev && block->prev->free)
        coalesce(block->prev); // merge with prev
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
        printf("Block %p | size: %zu | %s\n",(void *)current, current->size, current->free ? "free" : "used");
        current = current->next;
    }
}