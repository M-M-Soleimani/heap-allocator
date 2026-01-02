#include "../include/pool.h"
#include "../include/heap.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BLOCK_MAGIC 0xDEADBEEF

static void pool_init(memory_pool_t *pool, size_t block_size, size_t count)
{
    pool->block_size = block_size;
    pool->total_blocks = count;
    pool->pool_start = calloc(count, sizeof(pool_block_t) + block_size);
    if (!pool->pool_start)
    {
        errno = ENOMEM;
        pool->free_list = NULL;
        return;
    }

    pool->free_list = (pool_block_t *)pool->pool_start;
    pool_block_t *current = pool->free_list;

    for (size_t i = 0; i < count; i++)
    {
        current->magic = BLOCK_MAGIC;
        if (i < count - 1)
            current->next = (pool_block_t *)((char *)current + sizeof(pool_block_t) + block_size);
        else
            current->next = NULL;
        current = current->next;
    }
}

void pool_init_all(memory_pool_t pools[NUM_POOLS])
{
    for (int i = 0; i < NUM_POOLS; i++)
    {
        pool_init(&pools[i], POOL_BLOCK_SIZES[i], POOL_BLOCK_COUNTS[i]);
    }
}

void pool_destroy_all(memory_pool_t pools[NUM_POOLS])
{
    for (int i = 0; i < NUM_POOLS; i++)
    {
        free(pools[i].pool_start);
        pools[i].pool_start = NULL;
        pools[i].free_list = NULL;
    }
}

void *pool_alloc(memory_pool_t pools[NUM_POOLS], size_t size)
{
    for (int i = 0; i < NUM_POOLS; i++)
    {
        if (size <= pools[i].block_size && pools[i].free_list)
        {
            pool_block_t *block = pools[i].free_list;
            pools[i].free_list = block->next;
            block->next = NULL;
            return (void *)((char *)block + sizeof(pool_block_t));
        }
    }
    return NULL; // not handled, fallback heap
}

int pool_free(memory_pool_t pools[NUM_POOLS], void *ptr)
{
    if (!ptr)
        return 1;
    pool_block_t *block = (pool_block_t *)((char *)ptr - sizeof(pool_block_t));

    for (int i = 0; i < NUM_POOLS; i++)
    {
        uintptr_t start = (uintptr_t)pools[i].pool_start;
        uintptr_t end = start + (pools[i].block_size + sizeof(pool_block_t)) * pools[i].total_blocks;
        uintptr_t p = (uintptr_t)block;
        if (p >= start && p < end)
        {
            block->next = pools[i].free_list;
            pools[i].free_list = block;
            return 0; // successfully freed to pool
        }
    }
    return 1; // fallback heap
}

void pool_dump_all(memory_pool_t pools[NUM_POOLS])
{
    printf("Memory Pools:\n");
    for (int i = 0; i < NUM_POOLS; i++)
    {
        size_t used = 0;
        pool_block_t *current = pools[i].free_list;
        while (current)
        {
            used++;
            current = current->next;
        }
        printf("Pool %d | block size: %zu | total: %zu | free: %zu\n", i, pools[i].block_size, pools[i].total_blocks, used);
    }
}