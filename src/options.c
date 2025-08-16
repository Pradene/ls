#include "ls.h"

extern Options options;
extern SortType sort_type;
extern ShowType show_type;

int process_options(int ac, char **av) {
	int process_flag = 1;
	
	for (int i = 1; i < ac; i++) {
		if (process_flag && av[i][0] == '-' && av[i][1] != '\0') {
			if (ft_strcmp(av[i], "--") == 0) {
				process_flag = 0;
			} else {
				char *opt = av[i];
				while (*(++opt)) {
					switch (*opt) {
						case 'l': options |= LIST; break;
                        case 'g': options |= LIST_GROUP_ONLY; break;
						case 'R': options |= RECURSE; break;
						case 'r': options |= REVERSE; break;
						case 'a': show_type = SHOW_ALL; break;
						case 'f': show_type = SHOW_ALL; sort_type = SORT_NONE; break;
						case 'A': show_type = SHOW_ALMOST_ALL; break;
						case 't': sort_type = SORT_TIME; break;
						case 'S': sort_type = SORT_SIZE; break;
						case 'U': sort_type = SORT_NONE; break;
						default:
							fprintf(stderr, "Invalid option: '%c'\n", *opt);
							return (-1);
					}
				}
			}
		}
	}
	return (0);
}

int process_names(int ac, char **av, char ***names) {
	*names = malloc((ac - 1) * sizeof(char *));
	if (!*names) {
		fprintf(stderr, "malloc failed\n");
		return (-1);
	}
	
	int count = 0;
	int process_flag = 1;
	
	for (int i = 1; i < ac; i++) {
		if (process_flag && av[i][0] == '-' && av[i][1] != '\0') {
			if (ft_strcmp(av[i], "--") == 0) {
				process_flag = 0;
			}
		} else {
			(*names)[count++] = av[i];
		}
	}
	return (count);
}