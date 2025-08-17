#include "ls.h"

Options options = NONE;
SortType sort_type = SORT_NAME;
ShowType show_type = SHOW_VISIBLE;

int main(int ac, char **av) {	
	if (process_options(ac, av) != 0) {
		return (EXIT_FAILURE);
	}
	
	char **names = NULL;
	int names_count = process_names(ac, av, &names);
	if (names_count == -1) {
		return (EXIT_FAILURE);
	}
	
	if (names_count == 0) {
		process_directory(".");
	} else {
		quicksort(names, names_count, sizeof(char *), compare_name);
		if (options & REVERSE) {
			reverse(names, names_count, sizeof(char *));
		}

		for (int i = 0; i < names_count; i++) {
			if (i != 0) printf("\n");
			process_directory(names[i]);
		}
	}
	
	free(names);
	return (EXIT_SUCCESS);
}