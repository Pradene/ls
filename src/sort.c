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

int compare_file_name(const void *a, const void *b) {
    const t_file *file_a = *(const t_file **)a;
    const t_file *file_b = *(const t_file **)b;
    const char *name_a = file_a->name;
    const char *name_b = file_b->name;

    // For other entries, skip ALL leading dots before comparison
    const char *str1 = name_a;
    const char *str2 = name_b;

    while (*str1 == '.') str1++;
    while (*str2 == '.') str2++;

    // Handle empty strings after skipping dots
    if (*str1 == '\0' && *str2 == '\0') return 0;
    if (*str1 == '\0') return -1;
    if (*str2 == '\0') return 1;

    // Case-insensitive comparison
    char *l1 = ft_strtolower(str1);
    char *l2 = ft_strtolower(str2);
    int result = ft_strcmp(l1, l2);
    
    free(l1);
    free(l2);

    return result;
}

int compare_file_mtime(const void *a, const void *b) {
    return (*(t_file **)b)->stat.st_mtime - (*(t_file **)a)->stat.st_mtime;
}

int compare_name(const void *a, const void *b) {
    const char *name_a = *(const char **)a;
    const char *name_b = *(const char **)b;

    // For other entries, skip ALL leading dots before comparison
    const char *str1 = name_a;
    const char *str2 = name_b;

    while (*str1 == '.') str1++;
    while (*str2 == '.') str2++;

    // Handle empty strings after skipping dots
    if (*str1 == '\0' && *str2 == '\0') return 0;
    if (*str1 == '\0') return -1;
    if (*str2 == '\0') return 1;

    // Case-insensitive comparison
    char *l1 = ft_strtolower(str1);
    char *l2 = ft_strtolower(str2);
    int result = ft_strcmp(l1, l2);
    
    free(l1);
    free(l2);

    return result;
}