#ifndef STRATEGY_H
#define STRATEGY_H

#include <stddef.h>
#include "../include/heap.h"

typedef struct block_meta block_meta_t;

// Function pointer type for allocation strategy
typedef block_meta_t *(*alloc_strategy_func)(size_t size);

// Strategy functions
block_meta_t *first_fit(size_t size);
block_meta_t *best_fit(size_t size);  
block_meta_t *worst_fit(size_t size); 

// Pointer to current strategy, used by halloc
extern alloc_strategy_func current_strategy;

#endif // STRATEGY_H