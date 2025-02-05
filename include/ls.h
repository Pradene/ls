#ifndef DEFINE_LS_H
# define DEFINE_LS_H

# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>


typedef enum {
    NONE = 0,
    LIST = 1 << 0,
    RECURSE = 1 << 1,
    REVERSE = 1 << 2,
    ALL = 1 << 3,
    TIME = 1 << 4,
}   Options;

int ls(char *path, Options opts);

#endif