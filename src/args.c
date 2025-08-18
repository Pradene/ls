#include "ls.h"

extern Options options;
extern SortType sort_type;
extern ShowType show_type;

int parse_args_options(int ac, char **av) {
	bool process_flag = true;
	
	for (int i = 1; i < ac; i++) {
		if (process_flag == true && av[i][0] == '-' && av[i][1] != '\0') {
			if (ft_strcmp(av[i], "--") == 0) {
				process_flag = false;
			} else {
				char *opt = av[i];
				while (*(++opt)) {
					switch (*opt) {
						case 'l': options |= LIST; break;
						case 'g': options |= LIST_GROUP_ONLY; break;
						case 'R': options |= RECURSE; break;
						case 'r': options |= REVERSE; break;
						case 'd': options |= DIRECTORY; break;
						case 'u': options |= ACCESS_TIME; break;
						case 'a': show_type = SHOW_ALL; break;
						case 'f': show_type = SHOW_ALL; sort_type = SORT_NONE; break;
						case 'A': show_type = SHOW_ALMOST_ALL; break;
						case 't': sort_type = SORT_MTIME; break;
						case 'S': sort_type = SORT_SIZE; break;
						case 'U': sort_type = SORT_NONE; break;
						default:
							fprintf(stderr, "ft_ls: invalid option -- '%c'\n", *opt);
							return (1);
					}
				}
			}
		}
	}
	
	if (options & ACCESS_TIME) {
		if (options & LIST) {
			if (sort_type == SORT_MTIME) {
				sort_type = SORT_ATIME;
			}
		} else {
			if (sort_type == SORT_NAME) {
				sort_type = SORT_ATIME;
			}
		}
	}

	return (0);
}

int parse_args_files(int ac, char **av, DynamicArray **names) {
	*names = da_create(8);
	if (*names == NULL) {
		fprintf(stderr, "ft_ls: malloc failed\n");
		return (1);
	}

	bool process_flag = true;

	for (int i = 1; i < ac; i++) {
		if (process_flag == true && av[i][0] == '-' && av[i][1] != '\0') {
			if (ft_strcmp(av[i], "--") == 0) {
				process_flag = false;
			}
		} else {
			if (!da_push(*names, av[i])) {
				da_destroy(*names, NULL);
				fprintf(stderr, "ft_ls: failed to add name to array\n");
				return (1);
			}
		}
	}
	return (0);
}