#include "strategy.h"
#include "../include/heap.h"

// Current allocation strategy (default: First Fit)
alloc_strategy_func current_strategy = NULL;

// Return the first free block large enough (First Fit)
block_meta_t *first_fit(size_t size)
{
    block_meta_t *current = heap_start;
    while (current)
    {
        if (current->free && current->size >= size)
            return current;
        current = current->next;
    }
    return NULL;
}

// Return the smallest free block that fits (Best Fit)
block_meta_t *best_fit(size_t size)
{
    block_meta_t *current = heap_start;
    block_meta_t *best = NULL;
    while (current)
    {
        if (current->free && current->size >= size)
        {
            if (!best || current->size < best->size)
                best = current;
        }
        current = current->next;
    }
    return best;
}

// Return the largest free block (Worst Fit)
block_meta_t *worst_fit(size_t size)
{
    block_meta_t *current = heap_start;
    block_meta_t *worst = NULL;
    while (current)
    {
        if (current->free && current->size >= size)
        {
            if (!worst || current->size > worst->size)
                worst = current;
        }
        current = current->next;
    }
    return worst;
}