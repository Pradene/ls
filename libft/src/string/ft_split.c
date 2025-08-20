#include "libft.h"

static int	count(const char *s, char c) {
	int	i;
	int	count;

	i = 1;
	count = 0;
	while (s[i - 1]) {
		if ((s[i] == c || s[i] == 0) && s[i - 1] != c) {
			count++;
		}
		i++;
	}
	return (count);
}

char	**ft_split(const char *s, char c) {
	char	**strings;
	int		len;
	int		i;
	int		j;

	strings = malloc((count(s, c) + 1) * sizeof(char *));
	len = 0;
	j = 0;
	i = 0;
	while (j < ft_strlen(s) + 1) {
		if (s[j] != c && s[j] != 0) {
			len++;
		}
		if (len && (s[j] == c || s[j] == 0)) {
			strings[i] = ft_strndup(&s[j] - len, len);
			len = 0;
			i++;
		}
		j++;
	}
	strings[i] = 0;
	return (strings);
}
