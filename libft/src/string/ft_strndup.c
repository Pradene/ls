#include "libft.h"

char	*ft_strndup(const char *str, int n) {
	char	*dest;
	int		i;

	i = 0;
	dest = malloc(sizeof(char) * (n + 1));
	if (!dest) {
		return (0);
	}
	while (i < n) {
		dest[i] = str[i];
		i++;
	}
	dest[i] = 0;
	return (dest);
}