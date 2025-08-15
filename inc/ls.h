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

typedef struct {
    char            *link_name;
    char            *name;
    struct stat     stat;
}   FileInfo;

typedef struct {
    int blocks;   // for st_nlink
    int size;     // for st_size
    int group;    // for group name
    int user;     // for user name
}   ColumnWidths;

typedef struct {
    FileInfo        **files;
    size_t          files_count;
    unsigned long   total_blocks;
    ColumnWidths    widths;
}   DirectoryInfo;

typedef enum {
    NONE    = 0,
    LIST    = 1 << 0,
    RECURSE = 1 << 1,
    REVERSE = 1 << 2,
    ALL     = 1 << 3,
}   Options;

typedef enum {
    SORT_NAME,  // Default sorting
    SORT_TIME,  // -t flag
    SORT_SIZE,  // -S flag
    SORT_NONE,  // -U flag
}   SortType;

int ls(char *path);

void quicksort(void *base, size_t nmemb, size_t size, int (*comp)(const void *, const void *));

int compare_name(const void *a, const void *b);
int compare_file_name(const void *a, const void *b);
int compare_file_size(const void *a, const void *b);
int compare_file_mtime(const void *a, const void *b);

#endif
