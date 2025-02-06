#include "ls.h"
#include "libft.h"

void    perms(struct stat *st) {
    char perms[11];
    if (S_ISDIR(st->st_mode))      perms[0] = 'd';
    else if (S_ISCHR(st->st_mode)) perms[0] = 'c';
    else if (S_ISBLK(st->st_mode)) perms[0] = 'b';
    else if (S_ISFIFO(st->st_mode)) perms[0] = 'p';
    else if (S_ISLNK(st->st_mode)) perms[0] = 'l';
    else if (S_ISSOCK(st->st_mode)) perms[0] = 's';
    else perms[0] = '-';

    perms[1] = (st->st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st->st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st->st_mode & S_IXUSR) ? 
        ((st->st_mode & S_ISUID) ? 's' : 'x') : 
        ((st->st_mode & S_ISUID) ? 'S' : '-');

    perms[4] = (st->st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st->st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st->st_mode & S_IXGRP) ? 
        ((st->st_mode & S_ISGID) ? 's' : 'x') : 
        ((st->st_mode & S_ISGID) ? 'S' : '-');

    perms[7] = (st->st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st->st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st->st_mode & S_IXOTH) ? 
        ((st->st_mode & S_ISVTX) ? 't' : 'x') : 
        ((st->st_mode & S_ISVTX) ? 'T' : '-');
    perms[10] = '\0';

    printf("%s\n", perms);
}

int ls(char *path, Options opts) {
    DIR             *dir = NULL;
    struct dirent   *entry = NULL;
    char            **subdirs = NULL;
    int             subdir_count = 0;
    unsigned long   total = 0;
    
    dir = opendir(path);
    if (dir == NULL) {
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files if ALL is set
        if (!(opts & ALL) && entry->d_name[0] == '.') {
            continue ;
        }


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

        perms(&st);
        printf("hard link: %ld\n", st.st_nlink);
        printf("user name: %s\n", getpwuid(st.st_uid)->pw_name);
        printf("group name: %s\n", getgrgid(st.st_gid)->gr_name);
        printf("size: %lu\n", st.st_size);
        printf("time: %s", ctime(&st.st_mtime));
        printf("%s\n", entry->d_name);

        total += st.st_blocks;
    }

    printf("total %lu", total / 2);
    printf("\n\n");

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