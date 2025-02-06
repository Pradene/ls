#include "libft.h"

char    *ft_strtoupper(const char *s) {
    int     i;
    char    *upper;

    upper = malloc(ft_strlen(s) + 1);
    if (!upper) {
        return NULL;
    }
    i = 0;
    while (s && s[i]) {
        upper[i] = ft_toupper(s[i]);
        i++;
    }

    upper[i] = '\0';

    return upper;
}