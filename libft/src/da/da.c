#include "libft.h"

DynamicArray *da_create(size_t initial_capacity) {
	DynamicArray *arr = malloc(sizeof(DynamicArray));
	if (!arr) return (NULL);
	
	arr->data = malloc(initial_capacity * sizeof(void *));
	if (!arr->data) {
		free(arr);
		return (NULL);
	}
	
	arr->size = 0;
	arr->capacity = initial_capacity;
	return (arr);
}

bool da_push(DynamicArray *arr, void *element) {
	if (arr->size >= arr->capacity) {
		size_t capacity = arr->capacity * 2;
		void **data = malloc(capacity * sizeof(void *));
		if (!data) return (false);
		ft_memcpy(data, arr->data, arr->size * sizeof(void *));
		free(arr->data);
		arr->data = data;
		arr->capacity = capacity;
	}
	
	arr->data[arr->size++] = element;
	return (true);
}

void da_destroy(DynamicArray *arr, void (*free_func)(void *)) {
	if (!arr) return;
	
	if (free_func) {
		for (size_t i = 0; i < arr->size; i++) {
			if (arr->data[i]) {
				free_func(arr->data[i]);
			}
		}
	}
	
	free(arr->data);
	free(arr);
}

void **da_release(DynamicArray *arr) {
	if (!arr) return (NULL);
	
	void **data = arr->data;
	arr->data = NULL;
	free(arr);
	
	return (data);
}