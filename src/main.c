#include "ls.h"

int main(int ac, char **av) {
    Options options = NONE;
    char **names = malloc(ac * sizeof(char *)); // Maximum possible non-option arguments
    int name_count = 0;
    int process_options = 1;

    for (int i = 1; i < ac; i++) {
        if (process_options && av[i][0] == '-' && av[i][1] != '\0') {
            if (ft_strcmp(av[i], "--") == 0) {
                process_options = 0;
                continue;
            }

            // Process each option character
            char *opt = av[i] + 1;
            while (*opt) {
                switch (*opt) {
                    case 'l': options |= LIST; break;
                    case 'R': options |= RECURSE; break;
                    case 'r': options |= REVERSE; break;
                    case 'a': options |= ALL; break;
                    case 't': options |= TIME; break;
                    default:
                        fprintf(stderr, "Invalid option: '%c'\n", *opt);
                        free(names);
                        exit(EXIT_FAILURE);
                }
                opt++;
            }
        } else {
            // Add to names array
            names[name_count++] = av[i];
        }
    }

    // Resize names array to actual size (optional)
    names = realloc(names, name_count * sizeof(char *));

    quicksort(names, name_count, sizeof(char *), compare_name);

    if (name_count != 0) {
        for (int i = 0; i < name_count; i++) {
            ls(names[i], options);
        }
    } else {
        ls(".", options);
    }

    free(names);
    return EXIT_SUCCESS;
}