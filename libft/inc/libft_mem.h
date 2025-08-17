#ifndef MEM_H
# define MEM_H

void	ft_bzero(void *ptr, size_t n);
void	*ft_calloc(size_t n, size_t size);
void	*ft_memset(void *s, int value, size_t n);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_memmove(void *dest, const void *src, size_t n);
void	*ft_memchr(const void *mem, int c, size_t n);
int		ft_memcmp(const void *mem1, const void *mem2, size_t n);
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void	ft_striteri(char *s, void (*f)(unsigned int, char *));

#endif