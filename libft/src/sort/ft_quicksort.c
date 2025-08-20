#include "libft.h"

static void ft_quicksort_swap(void *a, void *b, size_t size) {
    char temp;
    char *p = a;
    char *q = b;

    for (size_t i = 0; i < size; ++i) {
        temp = p[i];
        p[i] = q[i];
        q[i] = temp;
    }
}

static ptrdiff_t ft_quicksort_partition(char *base, ptrdiff_t low, ptrdiff_t high, size_t size, int (*comp)(const void *, const void *)) {
    ptrdiff_t mid = low + (high - low) / 2;
    char *pivot = base + mid * size;
    ft_quicksort_swap(pivot, base + high * size, size);
    pivot = base + high * size;

    ptrdiff_t i = low - 1;
    for (ptrdiff_t j = low; j < high; ++j) {
        char *current = base + j * size;
        if (comp(current, pivot) <= 0) {
            ++i;
            ft_quicksort_swap(current, base + i * size, size);
        }
    }

    ft_quicksort_swap(base + (i + 1) * size, pivot, size);
    return (i + 1);
}

static void ft_quicksort_recurse(char *base, ptrdiff_t low, ptrdiff_t high, size_t size, int (*comp)(const void *, const void *)) {
    if (low < high) {
        ptrdiff_t pivot = ft_quicksort_partition(base, low, high, size, comp);
        ft_quicksort_recurse(base, low, pivot - 1, size, comp);
        ft_quicksort_recurse(base, pivot + 1, high, size, comp);
    }
}

void ft_quicksort(void *base, size_t nb, size_t size, int (*comp)(const void *, const void *)) {
    if (nb <= 1 || size == 0) {
		return;
	}
    char *base_ptr = (char *)base;
    ft_quicksort_recurse(base_ptr, 0, (ptrdiff_t)(nb - 1), size, comp);
}