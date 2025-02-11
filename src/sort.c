#include "ls.h"

// Swaps two elements of size 'size' bytes
static void swap(void *a, void *b, size_t size) {
    char temp;
    char *p = a;
    char *q = b;

    for (size_t i = 0; i < size; ++i) {
        temp = p[i];
        p[i] = q[i];
        q[i] = temp;
    }
}

// Lomuto's partition scheme with median-of-three pivot selection
static ptrdiff_t partition(char *base, ptrdiff_t low, ptrdiff_t high, size_t size, int (*comp)(const void *, const void *)) {
    // Choose middle element as pivot to avoid worst-case performance
    ptrdiff_t mid = low + (high - low) / 2;
    char *pivot = base + mid * size;
    // Move pivot to the end to fit Lomuto's scheme
    swap(pivot, base + high * size, size);
    pivot = base + high * size;

    ptrdiff_t i = low - 1;
    for (ptrdiff_t j = low; j < high; ++j) {
        char *current = base + j * size;
        if (comp(current, pivot) <= 0) {
            ++i;
            swap(current, base + i * size, size);
        }
    }

    // Move pivot to its correct position
    swap(base + (i + 1) * size, pivot, size);
    return i + 1; // Return pivot index
}

static void quicksort_recurse(char *base, ptrdiff_t low, ptrdiff_t high, size_t size, int (*comp)(const void *, const void *)) {
    if (low < high) {
        ptrdiff_t pivot = partition(base, low, high, size, comp);
        quicksort_recurse(base, low, pivot - 1, size, comp);
        quicksort_recurse(base, pivot + 1, high, size, comp);
    }
}

void quicksort(void *base, size_t nb, size_t size, int (*comp)(const void *, const void *)) {
    if (nb <= 1 || size == 0) return; // Nothing to sort
    char *base_ptr = (char *)base;
    quicksort_recurse(base_ptr, 0, (ptrdiff_t)(nb - 1), size, comp);
}
