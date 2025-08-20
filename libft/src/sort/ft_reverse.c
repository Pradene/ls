#include "libft.h"

void ft_reverse(void *base, size_t count, size_t size) {
	if (count <= 1) return;
	
	char *array = (char *)base;
	char *temp = malloc(size);
	if (!temp) return;
	
	for (size_t i = 0; i < count / 2; i++) {
		// Calculate positions
		char *left = array + (i * size);
		char *right = array + ((count - 1 - i) * size);
		
		// Swap elements
		ft_memcpy(temp, left, size);
		ft_memcpy(left, right, size);
		ft_memcpy(right, temp, size);
	}
	
	free(temp);
}