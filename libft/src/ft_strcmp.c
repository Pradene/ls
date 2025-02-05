#include "libft.h"

int	ft_strcmp(const char *str1, const char *str2)
{
	size_t i = 0;

	while (str1[i] && str2[i])
	{
		if ((unsigned char)str1[i] != (unsigned char)str2[i])
			return ((unsigned char)str1[i] - (unsigned char)str2[i]);
		i++;
	}
	
	return ((unsigned char)str1[i] - (unsigned char)str2[i]);
}