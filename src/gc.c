#include "../include/gc.h"
#include "../include/heap.h"

// Mark a block as reachable
static void mark(block_meta_t *block)
{
    if (!block || block->marked || block->free)
        return;
    block->marked = 1;
}

// Sweep heap and free unmarked blocks
static void sweep_heap()
{
    block_meta_t *current = heap_start;
    while (current)
    {
        if (!current->marked && !current->free)
        {
            // Unreachable block -> free it
            current->free = 1;
        }
        else
        {
            // Reset mark for next GC
            current->marked = 0;
        }
        current = current->next;
    }

    // Optional: coalesce consecutive free blocks
    current = heap_start;
    while (current && current->next)
    {
        if (current->free && current->next->free)
        {
            current->size += sizeof(block_meta_t) + current->next->size;
            current->next = current->next->next;
            if (current->next)
                current->next->prev = current;
        }
        else
        {
            current = current->next;
        }
    }
}

void gc_collect(void **roots, size_t root_count)
{
    // Mark phase: mark reachable blocks
    for (size_t i = 0; i < root_count; i++)
    {
        if (!roots[i])
            continue;
        block_meta_t *b = (block_meta_t *)((char *)roots[i] - sizeof(block_meta_t));
        mark(b);
    }

    // Sweep phase: free unmarked blocks
    sweep_heap();
}