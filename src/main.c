#include "ls.h"

Options options = NONE;
SortType sort_type = SORT_NAME;
ShowType show_type = SHOW_VISIBLE;

int main(int ac, char **av) {	
	if (parse_args_options(ac, av) != 0) {
		return (EXIT_FAILURE);
	}
	
	DynamicArray *files = NULL;
	if (parse_args_files(ac, av, &files) != 0) {
		return (EXIT_FAILURE);
	}
	
	size_t files_count = da_size(files);
	if (files_count == 0) {
		process_directory(".");
	} else {
		quicksort(files, files_count, sizeof(char *), compare_name);
		if (options & REVERSE) {
			reverse(files, files_count, sizeof(char *));
		}

		for (size_t i = 0; i < files_count; i++) {
			if (i != 0) printf("\n");
			char *name = (char *)da_get(files, i);
			process_directory(name);
		}
	}
	
	da_destroy(files, NULL);
	return (EXIT_SUCCESS);
}