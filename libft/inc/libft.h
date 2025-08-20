#ifndef LIBFT_H
#define LIBFT_H

#include "ft_printf.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

// Initial capacity of a dynamic array
#ifndef FT_DA_INIT_CAP
#define FT_DA_INIT_CAP 256
#endif

// Reserve some space for items - returns 0 on success, -1 on failure
#define ft_da_reserve(da, expected_capacity) ({                                        \
    int __result = 1;                                                                  \
    if ((expected_capacity) > (da)->capacity) {                                        \
        if ((da)->capacity == 0) {                                                     \
            (da)->capacity = FT_DA_INIT_CAP;                                           \
        }                                                                              \
        while ((expected_capacity) > (da)->capacity) {                                 \
            (da)->capacity *= 2;                                                       \
        }                                                                              \
        void *__pointer = malloc((da)->capacity * sizeof(*(da)->items));               \
        if (__pointer == NULL) {                                                       \
            __result = 0;                                                             \
        } else {                                                                       \
            if ((da)->items) {                                                         \
                ft_memcpy(__pointer, (da)->items, (da)->count * sizeof(*(da)->items)); \
                free((da)->items);                                                     \
            }                                                                          \
            (da)->items = __pointer;                                                   \
        }                                                                              \
    }                                                                                  \
    __result;                                                                          \
})

// Append an item to a dynamic array - returns 0 on success, -1 on failure
#define ft_da_append(da, item) ({                         \
    int __result = ft_da_reserve((da), (da)->count + 1);  \
    if (__result != 0) {                                  \
        (da)->items[(da)->count++] = (item);              \
    }                                                     \
    __result;                                             \
})

#define ft_da_free(da) ({ if ((da).items) free((da).items); })

// Append several items to a dynamic array - returns 0 on success, -1 on failure
#define ft_da_append_many(da, new_items, new_items_count) ({                                    \
    int __result = ft_da_reserve((da), (da)->count + (new_items_count));                        \
    if (__result != 0) {                                                                        \
        ft_memcpy((da)->items + (da)->count, (new_items), (new_items_count)*sizeof(*(da)->items)); \
        (da)->count += (new_items_count);                                                       \
    }                                                                                           \
    __result;                                                                                   \
})

// Resize dynamic array - returns 0 on success, -1 on failure
#define ft_da_resize(da, new_size) ({               \
    int __result = ft_da_reserve((da), (new_size)); \
    if (__result != 0) {                            \
        (da)->count = (new_size);                   \
    }                                               \
    __result;                                       \
})

// Remove items and put the last item at position of removed item
#define ft_da_remove_unordered(da, i) ({          \
    size_t j = (i);                               \
    (da)->items[j] = (da)->items[--(da)->count];  \
})

// Dynamic array iterator
#define ft_da_foreach(da, it, Type) for (Type *it = (da)->items; it < (da)->items + (da)->count; ++it)

// Dynamic array size
#define ft_da_size(da) (da)->count


// Char utility functions
int	ft_isalnum(int c);
int	ft_isspace(int c);
int	ft_isalpha(int c);
int	ft_isdigit(int c);
int	ft_isascii(int c);
int	ft_isprint(int c);
int	ft_toupper(int c);
int	ft_tolower(int c);


// Linked List
typedef struct List {
	void        *content;
	struct List	*next;
}	List;

List	*ft_lstnew(void *content);
void	ft_lstadd_front(List **lst, List *new);
int		ft_lstsize(List *lst);
List	*ft_lstlast(List *lst);
void	ft_lstadd_back(List **lst, List *new);
void	ft_lstdelone(List *lst, void (*del)(void*));
void	ft_lstclear(List **lst, void (*del)(void*));
void	ft_lstiter(List *lst, void (*f)(void *));
List	*ft_lstmap(List *lst, void *(*f)(void *), void (*del)(void *));


// Memory utility functions
void	ft_bzero(void *ptr, size_t n);
void	*ft_calloc(size_t n, size_t size);
void	*ft_memset(void *s, int value, size_t n);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_memmove(void *dest, const void *src, size_t n);
void	*ft_memchr(const void *mem, int c, size_t n);
int		ft_memcmp(const void *mem1, const void *mem2, size_t n);

// Print functions
void	ft_putchar_fd(char c, int fd);
void	ft_putchar(char c);
void	ft_putstr_fd(char *s, int fd);
void	ft_putstr(char *s);
void	ft_putendl_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);

// Sorting functions
void ft_quicksort(void *base, size_t nb, size_t size, int (*comp)(const void *, const void *));
void ft_reverse(void *base, size_t count, size_t size);


// String functions
int		ft_atoi(const char *str);
char	*ft_itoa(int n);
char	**ft_split(char const *s, char c);
char	*ft_strcat(char *dest, const char *src);
char	*ft_strcpy(char *dest, const char *src);
char	*ft_strdup(const char *s);
int		ft_strlen(const char *str);
size_t	ft_strlcpy(char *dest, const char *src, size_t n);
size_t	ft_strlcat(char *dest, const char *src, size_t n);
char	*ft_strchr(const char *str, int c);
char	*ft_strrchr(const char *str, int c);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strnstr(const char *s1, const char *s2, size_t n);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strtrim(char const *s1, char const *set);
void	ft_strtolower(char *s);
char	*ft_strndup(const char *str, int n);
char	*ft_substr(const char *s, unsigned int start, size_t n);
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void	ft_striteri(char *s, void (*f)(unsigned int, char *));



#endif
