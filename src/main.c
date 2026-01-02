#include "heap.h"
#include "strategy.h"
#include "gc.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define HEAP_SIZE (1024 * 8)

static void print_errno(const char *msg)
{
    printf("[ERRNO] %s | errno=%d (%s)\n", msg, errno, strerror(errno));
}

/* Basic allocation/free */
static void test_basic_alloc(void)
{
    printf("\n[TEST] Basic allocation\n");

    void *a = halloc(64);
    void *b = halloc(128);

    if (!a || !b)
        print_errno("basic alloc failed");

    heap_dump();

    hfree(a);
    hfree(b);

    heap_dump();
}

/* Split & coalesce */
static void test_split_coalesce(void)
{
    printf("\n[TEST] Split & Coalesce\n");

    void *a = halloc(200);
    void *b = halloc(200);

    heap_dump();

    hfree(a);
    heap_dump();

    hfree(b);
    heap_dump(); // should coalesce
}

/* Allocation strategies */
static void test_strategies(void)
{
    printf("\n[TEST] Allocation strategies\n");

    void *a, *b, *c;

    /* First Fit */
    current_strategy = first_fit;
    printf("Strategy: First Fit\n");
    a = halloc(100);
    b = halloc(100);
    hfree(a);
    c = halloc(80);
    heap_dump();

    hfree(b);
    hfree(c);

    /* Best Fit */
    current_strategy = best_fit;
    printf("Strategy: Best Fit\n");
    a = halloc(120);
    b = halloc(60);
    hfree(a);
    c = halloc(50);
    heap_dump();

    hfree(b);
    hfree(c);

    /* Worst Fit */
    current_strategy = worst_fit;
    printf("Strategy: Worst Fit\n");
    a = halloc(120);
    b = halloc(60);
    hfree(a);
    c = halloc(50);
    heap_dump();

    hfree(b);
    hfree(c);
}

/* Double free detection */
static void test_double_free(void)
{
    printf("\n[TEST] Double free\n");

    void *a = halloc(64);
    hfree(a);

    if (hfree(a) != HEAP_OK)
        print_errno("double free detected");
}

/* Invalid pointer */
static void test_invalid_pointer(void)
{
    printf("\n[TEST] Invalid pointer\n");

    int x;
    if (hfree(&x) != HEAP_OK)
        print_errno("invalid pointer detected");
}

/* Out of memory */
static void test_out_of_memory(void)
{
    printf("\n[TEST] Out of memory\n");

    void *a = halloc(HEAP_SIZE * 10);
    if (!a)
        print_errno("OOM detected");
}

/* Heap spraying heuristic */
static void test_heap_spray_pattern(void)
{
    printf("\n[TEST] Heap spraying pattern\n");

    void *blocks[64];
    for (int i = 0; i < 64; i++)
        blocks[i] = halloc(32);

    heap_dump();

    for (int i = 0; i < 64; i++)
        hfree(blocks[i]);
}

static void test_garbage_collector(void)
{
    printf("\n[TEST] Garbage Collector\n");

    // Allocate three blocks
    void *a = halloc(64);
    void *b = halloc(128); // will be garbage
    void *c = halloc(256);

    printf("Before GC:\n");
    heap_dump();

    // Only a and c are roots
    void *roots[] = {a, c};
    gc_collect(roots, 2);

    printf("After GC:\n");
    heap_dump();

    // Free remaining blocks
    hfree(a);
    hfree(c);
}


int main(void)
{
    printf("=== Custom Heap Allocator Test ===\n");

    if (hinit(HEAP_SIZE) != HEAP_OK)
    {
        print_errno("heap init failed");
        return 1;
    }

    test_basic_alloc();
    test_split_coalesce();
    test_strategies();
    test_double_free();
    test_invalid_pointer();
    test_out_of_memory();
    test_heap_spray_pattern();
    test_garbage_collector(); 

    hdestroy();

    printf("\n=== All tests completed ===\n");
    return 0;
}