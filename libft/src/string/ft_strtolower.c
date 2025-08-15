#include "libft.h"

void ft_strtolower(char *s) {
    int i = -1;
    
    while (s && s[++i]) {
        s[i] = ft_tolower(s[i]);
    }
}