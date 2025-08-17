#ifndef DIRECTORY_H
# define DIRECTORY_H

# include <sys/stat.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
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
	char		*path;
	FileInfo	**files;
	size_t		files_count;
}   DirectoryInfo;

void            process_directory(char *path);

#endif