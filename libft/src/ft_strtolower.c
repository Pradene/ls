#include "libft.h"

char    *ft_strtolower(const char *s) {
    int     i;
    char    *upper;

    upper = malloc(ft_strlen(s) + 1);
    if (!upper) {
        return NULL;
    }
    i = 0;
    while (s && s[i]) {
        upper[i] = ft_tolower(s[i]);
        i++;
    }

    upper[i] = '\0';

    return upper;
}
