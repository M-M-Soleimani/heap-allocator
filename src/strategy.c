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