#ifndef SORT_H
# define SORT_H

void quicksort(void *base, size_t nb, size_t size, int (*comp)(const void *, const void *));
void reverse(void *base, size_t count, size_t size);

#endif 