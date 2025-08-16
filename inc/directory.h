#ifndef DIRECTORY_H
# define DIRECTORY_H

# include <sys/stat.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <dirent.h>
# include <unistd.h>
# include <pwd.h>
# include <grp.h>

# include "options.h"

typedef struct {
	char		*link_name;
	char		*name;
	struct stat	stat;
}   FileInfo;

typedef struct {
	int nlink;
	int size;
	int group;
	int user;
}   ColumnWidths;

typedef struct {
	FileInfo		**files;
	size_t			files_count;
	unsigned long	total_blocks;
	ColumnWidths	widths;
}   DirectoryInfo;

int             should_skip_file(const char *name, ShowType show_type);
FileInfo        *create_file_info(const char *name, const char *full_path, struct stat *st);
void            update_directory_stats(DirectoryInfo *data, FileInfo *file);
DirectoryInfo   read_directory(char *path);
void            process_directory(char *path);
void            free_file(FileInfo *file);
void            free_files(FileInfo **files, size_t count);

#endif