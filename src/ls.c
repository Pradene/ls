#include "ls.h"

static Options options = NONE;
static SortType sort_type = SORT_NAME;
static ShowType show_type = SHOW_NORMAL;
static ColumnWidths widths = {0};

static int int_len(int n) {
    int i = 1;
    while ((n = n / 10) != 0) {
        ++i;
    }
    return (i);
}

static void colored_name(const FileInfo *file, char *buffer, size_t buffer_size) {
    const char *color = RESET;
    if (S_ISDIR(file->stat.st_mode))        color = BLUE;
    else if (S_ISFIFO(file->stat.st_mode))  color = CYAN;
    else if (S_ISLNK(file->stat.st_mode))   color = CYAN;
    else if (file->stat.st_mode & S_IXUSR)  color = GREEN;

    snprintf(buffer, buffer_size, "%s%s%s", color, file->name, RESET);
}

static void format_date(const FileInfo *file, char *buffer) {
    time_t now = time(NULL);
    time_t file_time = file->stat.st_mtime;
    struct tm *now_tm = localtime(&now);
    struct tm *file_tm = localtime(&file_time);
    
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    
    time_t six_months = 180 * 24 * 60 * 60;
    int is_recent = (file_time >= now - six_months) && 
                    (file_time <= now + six_months) &&
                    (file_tm->tm_year == now_tm->tm_year);

    if (is_recent) {
        snprintf(
            buffer, 13, "%s %2d %02d:%02d",
            months[file_tm->tm_mon],
            file_tm->tm_mday,
            file_tm->tm_hour,
            file_tm->tm_min
        );
    } else {
        snprintf(
            buffer, 13, "%s %2d  %04d",
            months[file_tm->tm_mon],
            file_tm->tm_mday,
            file_tm->tm_year + 1900
        );
    }
}

static void format_permissions(FileInfo *file, char *buffer) {
    mode_t mode = file->stat.st_mode;
    char type = '-';

    if (S_ISDIR(mode))       type = 'd';
    else if (S_ISCHR(mode))  type = 'c';
    else if (S_ISBLK(mode))  type = 'b';
    else if (S_ISFIFO(mode)) type = 'p';
    else if (S_ISLNK(mode))  type = 'l';
    else if (S_ISSOCK(mode)) type = 's';

    buffer[0] = type;
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

    buffer[10] = (listxattr(file->name, NULL, 0) > 0) ? '@' : ' ';
    buffer[11] = '\0';
}

static void print_directory(DirectoryInfo directory, const char *path) {
    size_t buf_size = 4096;
    char *output_buf = malloc(buf_size);
    if (!output_buf) {
        return;
    }
    size_t pos = 0;

    // Print header for recursive mode
    if (options & LIST) {
        if (options & RECURSE) {
            pos += snprintf(output_buf + pos, buf_size - pos, "%s:\ntotal %lu\n", path, directory.total_blocks);
        } else {
            pos += snprintf(output_buf + pos, buf_size - pos, "total %lu\n", directory.total_blocks);
        }
    }

    // Process each file
    for (size_t i = 0; i < directory.files_count; i++) {
        FileInfo *file = directory.files[i];
        char date_buf[64], perm_buf[64], name_buf[256];
        colored_name(file, name_buf, sizeof(name_buf));

        if (options & LIST) {
            const char *link_indicator = file->link_name ? " -> " : "";
            const char *link_target = file->link_name ? file->link_name : "";

            format_date(file, date_buf);
            format_permissions(file, perm_buf);

            // Format the line
            int line_len = snprintf(NULL, 0, "%s %*ld %-*s %-*s %*lu %s %s%s%s\n",
                perm_buf, directory.widths.blocks, file->stat.st_nlink,
                directory.widths.user, getpwuid(file->stat.st_uid)->pw_name,
                directory.widths.group, getgrgid(file->stat.st_gid)->gr_name,
                directory.widths.size, file->stat.st_size, date_buf, name_buf, 
                link_indicator, link_target);

            // Resize buffer if needed
            if (pos + line_len + 1 > buf_size) {
                buf_size *= 2;
                output_buf = realloc(output_buf, buf_size);
            }

            // Add to output buffer
            pos += snprintf(output_buf + pos, buf_size - pos, 
                "%s %*ld %-*s %-*s %*lu %s %s%s%s\n",
                perm_buf, directory.widths.blocks, file->stat.st_nlink,
                directory.widths.user, getpwuid(file->stat.st_uid)->pw_name,
                directory.widths.group, getgrgid(file->stat.st_gid)->gr_name,
                directory.widths.size, file->stat.st_size, date_buf, name_buf, 
                link_indicator, link_target);
        } else {
            pos += snprintf(output_buf + pos, buf_size - pos, "%s  ", name_buf);
            if (directory.files_count - 1 == i) {
                pos += snprintf(output_buf + pos, buf_size - pos, "\n");
            }
        }
    }

    fwrite(output_buf, 1, pos, stdout);
    free(output_buf);
}

static void free_file(FileInfo *file) {
    if (file) {
        free(file->link_name);
        free(file->name);
        free(file);
    }
}

static void free_files(FileInfo **files, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free_file(files[i]);
    }
    free(files);
}

static DirectoryInfo read_directory(char *path) {
    DirectoryInfo data = {0};
    DIR *dir = opendir(path);
    if (!dir) {
        return (data);
    }

    size_t capacity = 16;
    data.files = malloc(capacity * sizeof(FileInfo *));
    if (!data.files) {
        return (data);
    }
    unsigned long total_blocks = 0;

    // Process directory entries
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Handle hidden file display based on show_type
        int is_hidden = (entry->d_name[0] == '.');
        int is_special_dir = (!ft_strcmp(entry->d_name, ".") || !ft_strcmp(entry->d_name, ".."));
        
        if (show_type == SHOW_NORMAL && is_hidden) {
            continue; // Skip all hidden files
        } else if (show_type == SHOW_ALMOST_ALL && is_special_dir) {
            continue; // Skip . and .. but show other hidden files
        }
        // SHOW_ALL shows everything, so no skip conditions

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (lstat(full_path, &st) != 0) {
            continue;
        }

        // Create file entry
        FileInfo *file = malloc(sizeof(FileInfo));
        file->name = ft_strdup(entry->d_name);
        file->link_name = NULL;
        file->stat = st;

        // Resize array if needed
        if (data.files_count >= capacity) {
            capacity *= 2;
            FileInfo **new_files = realloc(data.files, capacity * sizeof(FileInfo *));
            if (!new_files) {
                break;
            }
            data.files = new_files;
        }
        data.files[data.files_count++] = file;

        // Update column widths and totals
        total_blocks += st.st_blocks / 2;
        widths.blocks = MAX(widths.blocks, int_len(st.st_nlink));
        widths.size = MAX(widths.size, int_len(st.st_size));

        char *user = getpwuid(st.st_uid)->pw_name;
        char *group = getgrgid(st.st_gid)->gr_name;
        widths.user = MAX(widths.user, ft_strlen(user));
        widths.group = MAX(widths.group, ft_strlen(group));

        // Handle symlinks
        if (S_ISLNK(st.st_mode)) {
            char *link_name = malloc(1024);
            if (readlink(full_path, link_name, 1024) > 0) {
                file->link_name = link_name;
            } else {
                free(link_name);
            }
        }
    }

    closedir(dir);
    data.total_blocks = total_blocks;
    data.widths = widths;
    return (data);
}

static void process_directory(char *path) {
    // Read directory contents
    DirectoryInfo data = read_directory(path);
    if (!data.files) {
        return;
    }
    
    // Sort files based on the current sort_type
    switch (sort_type) {
        case SORT_NONE: break;
        case SORT_TIME: quicksort(data.files, data.files_count, sizeof(FileInfo *), compare_file_mtime); break;
        case SORT_SIZE: quicksort(data.files, data.files_count, sizeof(FileInfo *), compare_file_size); break;
        case SORT_NAME:
        default: quicksort(data.files, data.files_count, sizeof(FileInfo *), compare_file_name); break;
    }

    // Print files
    print_directory(data, path);
    
    // Handle recursion
    if (options & RECURSE) {
        for (size_t i = 0; i < data.files_count; i++) {
            FileInfo *file = data.files[i];
            int is_special_dir = !ft_strcmp(file->name, ".") || !ft_strcmp(file->name, "..");
            
            if (S_ISDIR(file->stat.st_mode) && !is_special_dir) {                
                char sub_path[1024];
                snprintf(sub_path, sizeof(sub_path), "%s/%s", path, file->name);
                
                printf("\n");
                process_directory(sub_path);
            }
            free_file(file);
        }
        free(data.files);
    } else {
        free_files(data.files, data.files_count);
    }
}

static int process_options(int ac, char **av) {
    int process_flag = 1;
    
    for (int i = 1; i < ac; i++) {
        if (process_flag && av[i][0] == '-' && av[i][1] != '\0') {
            if (ft_strcmp(av[i], "--") == 0) {
                process_flag = 0;
                continue;
            }
            
            char *opt = av[i] + 1;
            while (*opt) {
                switch (*opt) {
                    case 'l': options |= LIST; break;
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
                opt++;
            }
        }
    }
    return (0);
}

static int process_names(int ac, char **av, char ***names) {
    *names = malloc((ac - 1) * sizeof(char *));
    if (!*names) {
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

int main(int ac, char **av) {
    char **names = NULL;
    int names_count;
    
    if (process_options(ac, av) != 0) {
        return (EXIT_FAILURE);
    }

    names_count = process_names(ac, av, &names);
    if (names_count == -1) {
        return (EXIT_FAILURE);
    }
    
    if (names_count > 0) {
        quicksort(names, names_count, sizeof(char *), compare_name);

        for (int i = 0; i < names_count; i++) {
            if (i != 0) {
                printf("\n");
            }
            process_directory(names[i]);
        }
    } else {
        process_directory(".");
    }
    
    free(names);
    return (EXIT_SUCCESS);
}