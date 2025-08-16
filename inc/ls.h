#ifndef DEFINE_LS_H
# define DEFINE_LS_H

# include <sys/types.h>
# include <sys/stat.h>
# include <sys/xattr.h>
# include <sys/ioctl.h>

# include <dirent.h>
# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>
# include <unistd.h>
# include <pwd.h>
# include <grp.h>
# include <time.h>
# include <termios.h>

# include "libft.h"

# define BLUE  "\x1B[34m"
# define CYAN  "\x1B[36m"
# define GREEN "\x1B[32m"
# define RESET "\x1B[0m"

# define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct {
	char		*link_name;
	char		*name;
	struct stat	stat;
}   FileInfo;

typedef struct {
	int blocks;	// for st_nlink
	int size;	// for st_size
	int group;	// for group name
	int user;	// for user name
}   ColumnWidths;

typedef struct {
	FileInfo		**files;
	size_t			files_count;
	unsigned long	total_blocks;
	ColumnWidths	widths;
}   DirectoryInfo;


typedef struct {
	char	**display_names;
	int		*widths;
	size_t	count;
}   DisplayData;

typedef struct {
	int cols;
	int rows;
	int *col_widths;
	int total_width;
	int valid;
}   LayoutInfo;

typedef struct {
	char	*buffer;
	size_t	size;
	size_t	pos;
}   StringBuffer;

typedef enum {
	NONE	= 0,
	LIST	= 1 << 0,  // -l flag
	RECURSE	= 1 << 1,  // -R flag
	REVERSE	= 1 << 2,  // -r flag
}   Options;

typedef enum {
	SORT_NAME,  // Default sorting
	SORT_TIME,  // -t flag
	SORT_SIZE,  // -S flag
	SORT_NONE,  // -U flag
}   SortType;

typedef enum {
	SHOW_NORMAL,      // Default: hide hidden files
	SHOW_ALL,         // -a flag: show all including . and ..
	SHOW_ALMOST_ALL,  // -A flag: show all except . and ..
}   ShowType;

int ls(char *path);

int compare_name(const void *a, const void *b);
int compare_file_name(const void *a, const void *b);
int compare_file_size(const void *a, const void *b);
int compare_file_mtime(const void *a, const void *b);

#endif
