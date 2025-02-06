#include "ls.h"
#include "libft.h"

char    *date(const time_t *timestamp) {
    char    *date = "";

    time_t  now = time(NULL);
    
    char    *file_time = ft_strdup(ctime(timestamp));
    char    *curr_time = ft_strdup(ctime(&now));

    char    *month = ft_substr(file_time, 4, 4);
    char    *day = ft_substr(file_time, 8, 3);
    char    *hour = ft_substr(file_time, 11, 5);

    char    *year = ft_substr(file_time, 20, 4);
    char    *curr = ft_substr(curr_time, 20, 4);

    date = ft_strjoin(date, month);
    date = ft_strjoin(date, day);

    if (ft_strcmp(year, curr) != 0) {
        date = ft_strjoin(date, year);
    } else {
        date = ft_strjoin(date, hour);
    }
    
    return date;
}

char    *permissions(struct stat *st) {
    char perms[11];

    if (S_ISDIR(st->st_mode))       perms[0] = 'd';
    else if (S_ISCHR(st->st_mode))  perms[0] = 'c';
    else if (S_ISBLK(st->st_mode))  perms[0] = 'b';
    else if (S_ISFIFO(st->st_mode)) perms[0] = 'p';
    else if (S_ISLNK(st->st_mode))  perms[0] = 'l';
    else if (S_ISSOCK(st->st_mode)) perms[0] = 's';
    else                            perms[0] = '-';

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

    return ft_strdup(perms);
}

int compare_name(const void *a, const void *b) {
    return ft_strcmp((*(t_file **)a)->name, (*(t_file **)b)->name);
}

int compare_time(const void *a, const void *b) {
    return (*(t_file **)b)->stat.st_mtime - (*(t_file **)a)->stat.st_mtime;
}

t_list *insert_sorted(t_list *head, t_file *new_file, int (*cmp)(const void *, const void *)) {
    t_list *new_node = malloc(sizeof(t_list));
    new_node->content = new_file;
    new_node->next = NULL;

    if (!head || cmp(new_node, head) < 0) {
        new_node->next = head;
        return new_node;
    }

    t_list *curr = head;
    while (curr->next && cmp(new_node, curr->next) > 0) {
        curr = curr->next;
    }
    
    new_node->next = curr->next;
    curr->next = new_node;
    return head;
}

void print_list(t_list *head) {
    t_list *temp = head;
    while (temp) {
        t_file *f = (t_file *)temp->content;
        printf("%s %ld %s %s %lu %s %s\n", \
            permissions(&f->stat), \
            f->stat.st_nlink, \
            getpwuid(f->stat.st_uid)->pw_name, \
            getgrgid(f->stat.st_gid)->gr_name, \
            f->stat.st_size, \
            date(&f->stat.st_mtime), \
            f->name \
        );
        temp = temp->next;
    }
}

// **Free list element**
void free_file(void *elem) {
    t_file  *file = (t_file *)elem;

    if (file) {
        free(file->name);
        free(file);
    }
}

int ls(char *path, Options opts) {
    DIR             *dir = NULL;
    struct dirent   *entry = NULL;
    t_list          *file_list = NULL;
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

        char    full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            t_file *new_file = malloc(sizeof(t_file));
            new_file->name = ft_strdup(entry->d_name);
            new_file->stat = st;

            if (opts & TIME) {
                file_list = insert_sorted(file_list, new_file, compare_time);

            } else {
                file_list = insert_sorted(file_list, new_file, compare_name);
            }
            
            total += st.st_blocks;
        }
    }

    closedir(dir);

    // Recursion for directories if RECURSE is set
    if (opts & RECURSE) {
        printf("%s:\n", path);
        printf("total %lu\n", total / 2);
        print_list(file_list);
        printf("\n");

        t_list *temp = file_list;
        while (temp) {
            t_file *f = (t_file *)temp->content;
            if (S_ISDIR(f->stat.st_mode) && ft_strcmp(f->name, ".") != 0 && ft_strcmp(f->name, "..") != 0) {
                char sub_path[1024];
                snprintf(sub_path, sizeof(sub_path), "%s/%s", path, f->name);
                ls(sub_path, opts);
            }
            temp = temp->next;
        }

        ft_lstclear(&file_list, free_file);

    } else {
        printf("total %lu\n", total / 2);
        print_list(file_list);

        ft_lstclear(&file_list, free_file);
    }

    return (0);
}
