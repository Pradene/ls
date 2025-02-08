#ifndef DEFINE_LS_H
# define DEFINE_LS_H

# include <sys/types.h>
# include <sys/stat.h>
# include <sys/xattr.h>

# include <dirent.h>
# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>
# include <unistd.h>
# include <pwd.h>
# include <grp.h>
# include <time.h>

# include "libft.h"

# define BLUE  "\x1B[34m"
# define CYAN  "\x1B[36m"
# define GREEN "\x1B[32m"
# define RESET "\x1B[0m"

# define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct s_file {
    char            *link_name;
    char            *name;
    struct stat     stat;
}   t_file;

typedef enum {
    NONE    = 0,
    LIST    = 1 << 0,
    RECURSE = 1 << 1,
    REVERSE = 1 << 2,
    ALL     = 1 << 3,
    TIME    = 1 << 4,
}   Options;

int ls(char *path, Options opts);

void quicksort(void *base, size_t nmemb, size_t size, int (*comp)(const void *, const void *));

int compare_name(const void *a, const void *b);
int compare_file_name(const void *a, const void *b);
int compare_file_mtime(const void *a, const void *b);

#endif
