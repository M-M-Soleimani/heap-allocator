#ifndef POOL_H
#define POOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define NUM_POOLS 3
static const size_t POOL_BLOCK_SIZES[NUM_POOLS] = {32, 64, 128};
static const size_t POOL_BLOCK_COUNTS[NUM_POOLS] = {64, 64, 32}; 

typedef struct pool_block
{
    struct pool_block *next;
    uint32_t magic; // integrity check
    // char data[0];
} pool_block_t;

typedef struct memory_pool
{
    size_t block_size;
    size_t total_blocks;
    pool_block_t *free_list;
    void *pool_start;
} memory_pool_t;

// Initialize all pools
void pool_init_all(memory_pool_t pools[NUM_POOLS]);

// Destroy all pools
void pool_destroy_all(memory_pool_t pools[NUM_POOLS]);

// Allocate from pools, return NULL if size too big or pool empty
void *pool_alloc(memory_pool_t pools[NUM_POOLS], size_t size);

// Free block to pool, return 1 if not pool (fallback heap)
int pool_free(memory_pool_t pools[NUM_POOLS], void *ptr);

// Debug dump
void pool_dump_all(memory_pool_t pools[NUM_POOLS]);

#endif