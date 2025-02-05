#include "ls.h"
#include "libft.h"

int ls(char *path, Options opts) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return 1;
    }

    struct dirent *entry;
    char **subdirs = NULL;
    int subdir_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files if ALL is set
        if (!(opts & ALL) && entry->d_name[0] == '.') {
            continue ;
        }

        printf("%s\n", entry->d_name);

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            if (ft_strcmp(entry->d_name, ".") != 0 && ft_strcmp(entry->d_name, "..") != 0) {
                subdirs = realloc(subdirs, (subdir_count + 1) * sizeof(char *));
                subdirs[subdir_count] = ft_strdup(full_path);
                subdir_count++;
            }
        }
    }

    closedir(dir);

    // Recursion for directories if RECURSE is set
    if (opts & RECURSE) {
        for (int i = 0; i < subdir_count; i++) {
            printf("\n%s:\n", subdirs[i]);
            ls(subdirs[i], opts);
            free(subdirs[i]);
        }

        free(subdirs);
    }

    return (0);
}