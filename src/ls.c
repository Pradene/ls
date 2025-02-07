#include "ls.h"

// static int  permissions_width = 0;
static int  blocks_size_width = 0;
static int  file_size_width = 0;
static int  group_width = 0;
static int  user_width = 0;

int int_len(int n) {
    int i = 1;
    while ((n = n / 10) != 0) {
        i++;
    }

    return i;
}

void colored_name(const t_file *file, char *buffer, size_t buffer_size) {
    const char *color = RESET;
    if (S_ISDIR(file->stat.st_mode))        color = BLUE;
    else if (S_ISFIFO(file->stat.st_mode))  color = CYAN;
    else if (S_ISLNK(file->stat.st_mode))   color = CYAN;
    else if (file->stat.st_mode & S_IXUSR)  color = GREEN;

    snprintf(buffer, buffer_size, "%s%s%s", color, file->name, RESET);
}

// char    *date(const time_t *timestamp) {
//     char    *date = "";

//     time_t  now = time(NULL);
    
//     char    *file_time = ft_strdup(ctime(timestamp));
//     char    *curr_time = ft_strdup(ctime(&now));

//     char    *month = ft_substr(file_time, 4, 4);
//     char    *day = ft_substr(file_time, 8, 3);
//     char    *hour = ft_substr(file_time, 11, 5);

//     char    *year = ft_substr(file_time, 20, 4);
//     char    *curr = ft_substr(curr_time, 20, 4);

//     date = ft_strjoin(date, month);
//     date = ft_strjoin(date, day);

//     if (ft_strcmp(year, curr) != 0) {
//         date = ft_strjoin(date, year);
//     } else {
//         date = ft_strjoin(date, hour);
//     }

//     free(curr_time);
//     free(file_time);
//     free(month);
//     free(day);
//     free(hour);
//     free(year);
//     free(curr);
    
//     return date;
// }

void date(const time_t *timestamp, char *buffer, size_t buffer_size) {
    struct tm *tm_info = localtime(timestamp);
    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);

    char file_year[5], now_year[5];
    strftime(file_year, sizeof(file_year), "%Y", tm_info);
    strftime(now_year, sizeof(now_year), "%Y", now_tm);

    if (ft_strcmp(file_year, now_year) == 0) {
        strftime(buffer, buffer_size, "%b %e %H:%M", tm_info); // e.g., "Jun 30 21:49"
    } else {
        strftime(buffer, buffer_size, "%b %e  %Y", tm_info);   // e.g., "Jun 30  1993"
    }
}

void permissions(t_file *file, char *buffer) {
    struct stat st = file->stat;
    mode_t mode = st.st_mode;

    buffer[0] = '-';
    if (S_ISDIR(mode))       buffer[0] = 'd';
    else if (S_ISCHR(mode))  buffer[0] = 'c';
    else if (S_ISBLK(mode))  buffer[0] = 'b';
    else if (S_ISFIFO(mode)) buffer[0] = 'p';
    else if (S_ISLNK(mode))  buffer[0] = 'l';
    else if (S_ISSOCK(mode)) buffer[0] = 's';

    buffer[1] = (mode & S_IRUSR) ? 'r' : '-';
    buffer[2] = (mode & S_IWUSR) ? 'w' : '-';
    buffer[3] = (mode & S_IXUSR) ? 
        ((mode & S_ISUID) ? 's' : 'x') : 
        ((mode & S_ISUID) ? 'S' : '-');
    buffer[4] = (mode & S_IRGRP) ? 'r' : '-';
    buffer[5] = (mode & S_IWGRP) ? 'w' : '-';
    buffer[6] = (mode & S_IXGRP) ?
        ((mode & S_ISGID) ? 's' : 'x') :
        ((mode & S_ISGID) ? 'S' : '-');
    buffer[7] = (mode & S_IROTH) ? 'r' : '-';
    buffer[8] = (mode & S_IWOTH) ? 'w' : '-';
    buffer[9] = (mode & S_IXOTH) ?
        ((mode & S_ISVTX) ? 't' : 'x') :
        ((mode & S_ISVTX) ? 'T' : '-');

    // perms[10] = (listxattr(file->name, NULL, 0) > 0) ? '@' : '\0';
    buffer[10] = '\0';
}

void print_files(t_file **files, size_t count, const char *path, unsigned long total) {
    size_t  buf_size = 4096;
    char    *output_buf = malloc(buf_size);
    size_t  pos = 0;

    if (path) {
        pos += snprintf(output_buf + pos, buf_size - pos, "%s:\ntotal %lu\n", path, total);
    } else {
        pos += snprintf(output_buf + pos, buf_size - pos, "total %lu\n", total);
    }

    for (size_t i = 0; i < count; i++) {
        t_file *f = files[i];
        char d_buf[64], p_buf[12], n_buf[256];
        const char *link_part = f->link_name ? " -> " : "";
        const char *link_name = f->link_name ? f->link_name : "";

        // Get data into stack buffers
        date(&f->stat.st_mtime, d_buf, sizeof(d_buf));
        permissions(f, p_buf);
        colored_name(f, n_buf, sizeof(n_buf));

        // Calculate line length and resize buffer if needed
        int line_len = snprintf(NULL, 0, "%s %*ld %-*s %-*s %*lu %s %s%s%s\n",
            p_buf, blocks_size_width, f->stat.st_nlink,
            user_width, getpwuid(f->stat.st_uid)->pw_name,
            group_width, getgrgid(f->stat.st_gid)->gr_name,
            file_size_width, f->stat.st_size, d_buf, n_buf, link_part, link_name);

        if (pos + line_len + 1 > buf_size) {
            buf_size *= 2;
            output_buf = realloc(output_buf, buf_size);
        }

        // Append to buffer
        pos += snprintf(output_buf + pos, buf_size - pos, "%s %*ld %-*s %-*s %*lu %s %s%s%s\n",
            p_buf, blocks_size_width, f->stat.st_nlink,
            user_width, getpwuid(f->stat.st_uid)->pw_name,
            group_width, getgrgid(f->stat.st_gid)->gr_name,
            file_size_width, f->stat.st_size, d_buf, n_buf, link_part, link_name);
    }

    if (path) {
        pos += snprintf(output_buf + pos, buf_size - pos, "\n");
    }

    fwrite(output_buf, 1, pos, stdout);
    free(output_buf);
}

void free_file(t_file *file) {
    if (file->link_name) {
        free(file->link_name);
    }

    free(file->name);
    free(file);
}

// **Free list element**
void free_files(t_file **files, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free_file(files[i]);
    }

    free(files);
}

int ls(char *path, Options opts) {
    DIR             *dir = NULL;
    struct dirent   *entry = NULL;
    t_file          **files = NULL;
    size_t          capacity = 16;
    size_t          count = 0;
    unsigned long   total = 0;

    dir = opendir(path);
    if (dir == NULL) {
        return 1;
    }

    files = malloc(capacity * sizeof(t_file *));
    if (!files) {
        closedir(dir);
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
        if (lstat(full_path, &st) == 0) {
            t_file *new_file = malloc(sizeof(t_file));
            new_file->name = ft_strdup(entry->d_name);
            new_file->link_name = NULL;
            new_file->stat = st;

            if (count >= capacity) {
                capacity *= 2;
                t_file **new_files = realloc(files, capacity * sizeof(t_file *));
                if (!new_files) {
                    perror("realloc");
                    break;
                }
                files = new_files;
            }

            files[count++] = new_file;

            // Update column widths and total blocks
            total += st.st_blocks / 2;
            blocks_size_width = MAX(blocks_size_width, int_len(st.st_nlink));
            file_size_width = MAX(file_size_width, int_len(st.st_size));

            char *user = getpwuid(st.st_uid)->pw_name;
            char *group = getgrgid(st.st_gid)->gr_name;
            user_width = MAX(user_width, ft_strlen(user));
            group_width = MAX(group_width, ft_strlen(group));

            // Handle symlinks
            if (S_ISLNK(st.st_mode)) {
                char *link_name = malloc(1024);
                if (readlink(full_path, link_name, 1024 - 1) == -1) {
                    free(link_name);
                    new_file->link_name = NULL;
                } else {
                    new_file->link_name = link_name;
                }
            }
        }
    }

    if (opts & TIME) {
        quicksort(files, count, sizeof(t_file *), compare_file_mtime);
    } else {
        quicksort(files, count, sizeof(t_file *), compare_file_name);
    }

    closedir(dir);

    print_files(files, count, path, total);
    
    // Recursion for directories if RECURSE is set
    if (opts & RECURSE) {
        for (size_t i = 0; i < count; i++) {
            t_file *f = files[i];
            if (S_ISDIR(f->stat.st_mode) && ft_strcmp(f->name, ".") != 0 && ft_strcmp(f->name, "..") != 0) {                
                char sub_path[1024];
                snprintf(sub_path, sizeof(sub_path), "%s/%s", path, f->name);
                
                if (ls(sub_path, opts) == 1) {
                    free_files(files, count);
                    exit(1);
                }
            }

            free_file(files[i]);
        }

        free(files);

    } else {
        free_files(files, count);
    }
    

    return (0);
}
