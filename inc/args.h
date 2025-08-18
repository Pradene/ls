#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	NONE	        = 0,
	LIST	        = 1 << 0,  // -l flag
	LIST_GROUP_ONLY = 1 << 1,  // -g flag
	RECURSE	        = 1 << 2,  // -R flag
	REVERSE	        = 1 << 3,  // -r flag
	DIRECTORY	    = 1 << 4,  // -d flag
	ACCESS_TIME     = 1 << 5   // -u flag
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

int parse_args_options(int ac, char **av);
int parse_args_files(int ac, char **av, DynamicArray **names);

#endif