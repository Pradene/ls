#ifndef LS_H
#define LS_H

#define BLUE  "\x1B[34m"
#define CYAN  "\x1B[36m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

#include "libft.h"

#include <sys/xattr.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>

typedef struct {
	const char	**items;
	size_t		count;
	size_t		capacity;
}	Files;

typedef struct {
	char		*link;
	char		*name;
	struct stat	stat;
}   FileInfo;

typedef struct {
    FileInfo    *items;
    size_t      count;
    size_t      capacity;
}   FilesInfo;

typedef struct {
	char		*path;
	FilesInfo	files;
}   DirectoryInfo;

typedef struct {
    char	*display_name;
	int		width;
}	DisplayInfo;

typedef struct {
	DisplayInfo *items;
	size_t count;
	size_t capacity;
} DisplayArray;

typedef struct {
    int cols;
	int rows;
	int *col_widths;
	int total_width;
	int valid;
}	LayoutInfo;

typedef struct {
    int nlink;
	int size;
	int group;
	int user;
}	ColumnWidths;

typedef enum {
	NONE	        = 0,
	LIST	        = 1 << 0,  // -l flag
	LIST_GROUP_ONLY = 1 << 1,  // -g flag
	RECURSE	        = 1 << 2,  // -R flag
	REVERSE	        = 1 << 3,  // -r flag
	DIRECTORY	    = 1 << 4,  // -d flag
	ACCESS_TIME     = 1 << 5,  // -u flag
}   Options;

typedef enum {
	SORT_NAME,  // Default sorting
	SORT_MTIME, // -t flag
	SORT_ATIME, // -u flag
	SORT_SIZE,  // -S flag
	SORT_NONE,  // -U flag
}   SortType;

typedef enum {
	SHOW_VISIBLE,     // Default: hide hidden files
	SHOW_ALL,         // -a flag: show all including . and ..
	SHOW_ALMOST_ALL,  // -A flag: show all except . and ..
}   ShowType;

bool    parse_args_options(int ac, char **av);
bool    parse_args_files(int ac, char **av, Files *names);

int     compare_name(const void *a, const void *b);
int     compare_file_name(const void *a, const void *b);
int     compare_file_size(const void *a, const void *b);
int     compare_file_mtime(const void *a, const void *b);
int     compare_file_atime(const void *a, const void *b);

void	print_formatted(DirectoryInfo *directory);
void	print_list_formatted(DirectoryInfo *directory);

void	process_directory(char *path);

#endif
