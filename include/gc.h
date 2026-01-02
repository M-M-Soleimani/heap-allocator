#ifndef GC_H
#define GC_H

#include <stddef.h>
#include "heap.h"

// Garbage Collection API
void gc_collect(void **roots, size_t root_count);

#endif