#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

typedef struct {
	char		*link_name;
	char		*name;
	struct stat	stat;
}   FileInfo;

typedef struct {
	char			*path;
	DynamicArray	*files;
}   DirectoryInfo;

void	process_directory(char *path);

#endif