# Custom Heap Allocator

A **custom heap allocator** implemented in C, demonstrating manual heap management for educational purposes. This project includes memory allocation strategies, garbage collection, memory pool support, and basic heap-spraying detection.

---

## Features

- **Heap Initialization**: Allocate a contiguous memory region using `mmap`.
- **Dynamic Allocation**: `halloc` and `hfree` for manual memory management.
- **Allocation Strategies**: First Fit, Best Fit, Worst Fit.
- **Chunk Management**: Metadata stored in each chunk (size, in-use flag, next pointer, marked flag for GC).
- **Split & Coalesce**: Automatically split large chunks and merge adjacent free chunks.
- **Memory Pool**: Optional pool for small allocations.
- **Garbage Collection**: Mark-and-sweep collector to free unreachable memory blocks.
- **Heap Spraying Detection**: Heuristic-based detection of abnormal allocation patterns.
- **Error Handling**: Detects double-free, invalid pointers, out-of-memory, and sets `errno`.

---

## File Structure

```

heap-allocator/
│
├── include/
│   ├── heap.h       # Heap API
│   ├── strategy.h   # Allocation strategies
│   ├── gc.h         # Garbage collector
│   └── pool.h       # Memory pool
│
├── src/
│   ├── heap.c
│   ├── strategy.c
│   ├── gc.c
│   ├── pool.c
│   └── main.c       # Comprehensive test program
│
└── CMakeLists.txt   # Build configuration

````

---

## Public API

```c
int hinit(size_t size);      // Initialize heap
void *halloc(size_t size);   // Allocate memory
void hfree(void *ptr);       // Free memory
void gc_collect(void **roots, size_t root_count); // Garbage collection
void memory_pool_init(size_t pool_size);         // Initialize memory pool
void *pool_alloc(size_t size);                   // Allocate from pool
void pool_free(void *ptr);                       // Free pool allocation
````

---

## Usage

1. **Include the headers**:

```c
#include "heap.h"
#include "gc.h"
#include "pool.h"
```

2. **Initialize the heap**:

```c
if (hinit(8192) != HEAP_OK) {
    perror("Heap initialization failed");
    return 1;
}
```

3. **Allocate memory**:

```c
int *x = (int *)halloc(sizeof(int));
```

4. **Use Garbage Collector**:

```c
void *roots[] = {x};
gc_collect(roots, 1);
```

5. **Free memory**:

```c
hfree(x);
```

---

## Build Instructions

Using **CMake**:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

Or using **GCC** directly:

```bash
gcc -o heap_test src/*.c -I include
./heap_test
```

---

## Testing

The provided `main.c` runs a comprehensive test suite:

* Basic allocation and free
* Split and coalesce
* Allocation strategies (First, Best, Worst Fit)
* Double-free and invalid pointer detection
* Out-of-memory handling
* Heap spraying heuristic
* Garbage collector with root scanning
* Memory pool allocation

---

## Implementation Details

### Chunk Metadata

```c
typedef struct block_meta_t {
    size_t size;            // User data size
    int free;               // 1 = free, 0 = allocated
    int marked;             // Mark flag for GC
    struct block_meta_t *next;
    struct block_meta_t *prev;
} block_meta_t;
```

* Metadata is **stored at the start** of each chunk.
* `halloc` returns pointer **after the metadata**.
* `marked` is used for **garbage collection**.

### Alignment

All allocations are aligned to **8 bytes**:

```c
#define ALIGN8(x) (((x) + 7) & ~7)
```

---

### Garbage Collection

* **Mark-and-sweep**: Mark all reachable blocks from `roots` array and sweep the rest.
* Only memory blocks **reachable from roots** are preserved.
* Unreachable memory is **freed and coalesced**.

---

### Memory Pool

* Pre-allocated pool for small allocations.
* Reduces fragmentation and speeds up repeated small allocations.

---

### Heap Spraying Detection

* Heuristic checks for large sequential allocations.
* Alerts to potentially malicious allocation patterns.

---

## Notes

* Single-heap implementation.
* Not thread-safe.
* Designed for educational purposes; **not production-ready**.
* Compiler warnings treated as errors for safer memory handling.

---

## References

* [Memory management - Wikipedia](https://en.wikipedia.org/wiki/Memory_management)
* [mmap man page](https://man7.org/linux/man-pages/man2/mmap.2.html)
* [Chunking (computing)](https://en.wikipedia.org/wiki/Chunking_%28computing%29)
* [C dynamic memory allocation](https://en.wikipedia.org/wiki/C_dynamic_memory_allocation)
