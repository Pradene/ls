#ifndef DISPLAY_H
# define DISPLAY_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>
# include <sys/ioctl.h>
# include <pwd.h>
# include <grp.h>
# include <sys/xattr.h>

typedef struct {
    char    **display_names;
	size_t  count;
	int     *widths;
}   DisplayData;

typedef struct {
	int cols;
	int rows;
	int *col_widths;
	int total_width;
	int valid;
}   LayoutInfo;

void    get_colored_name(const FileInfo *file, char *buffer, size_t buffer_size);
int     get_terminal_width(void);
int     get_display_width(const char *str);

void    print_formatted(DirectoryInfo directory);
void    print_list_formatted(DirectoryInfo directory);

void    format_date(const FileInfo *file, char *buffer);
void    format_permissions(FileInfo *file, char *buffer);

#endif