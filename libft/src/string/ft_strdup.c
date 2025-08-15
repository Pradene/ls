#include "libft.h"

char	*ft_strdup(const char *s) {
	char	*str;
	int		size;

	size = ft_strlen(s) + 1;
	str = malloc(sizeof(char) * (size));
	if (!str) {
		return (NULL);
	}
	str = ft_memcpy(str, s, size);
	return (str);
}
