#ifndef DISPLAY_H
# define DISPLAY_H

# include <sys/xattr.h>
# include <sys/ioctl.h>

# include <grp.h>
# include <pwd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>

typedef struct {
    char    **display_names;
	size_t  count;
	int     *widths;
}   DisplayInfo;

typedef struct {
	int cols;
	int rows;
	int *col_widths;
	int total_width;
	int valid;
}   LayoutInfo;

typedef struct {
	int nlink;
	int size;
	int group;
	int user;
}   ColumnWidths;

void    print_formatted(DirectoryInfo directory);
void    print_list_formatted(DirectoryInfo directory);

#endif