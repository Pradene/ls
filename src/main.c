#include "ls.h"

Options options = NONE;
SortType sort_type = SORT_NAME;
ShowType show_type = SHOW_VISIBLE;

int main(int ac, char **av) {	
	if (parse_args_options(ac, av) == false) {
		return (EXIT_FAILURE);
	}
	
	Files files = {0};
	if (parse_args_files(ac, av, &files) == false) {
		return (EXIT_FAILURE);
	}
	
	size_t files_count = ft_da_size(&files);
	if (files_count == 0) {
		process_directory(".");
	} else {
		ft_quicksort(&files.items, files_count, sizeof(char *), compare_name);
		if (options & REVERSE) {
			ft_reverse(&files.items, files_count, sizeof(char *));
		}

		for (size_t i = 0; i < files_count; i++) {
			if (i != 0) printf("\n");
			process_directory((char *)files.items[i]);
		}
	}
	
	ft_da_free(files);
	return (EXIT_SUCCESS);
}