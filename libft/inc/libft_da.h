#ifndef DA_H
# define DA_H

typedef struct {
    void	**data;
    size_t	size;
    size_t	capacity;
}	DynamicArray;

DynamicArray	*da_create(size_t initial_capacity);
bool 	da_push(DynamicArray *arr, void *element);
void 	da_destroy(DynamicArray *arr, void (*free_func)(void *));
void 	**da_release(DynamicArray *arr);

#endif