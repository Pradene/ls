#include "ls.h"

static Options options = NONE;
static SortType sort_type = SORT_NAME;
static ShowType show_type = SHOW_NORMAL;

static inline int number_len(long long n) {
	if (n == 0) {
		return (1);
	}
	
	int count = 0;
	if (n < 0) {
		count = 1; // For minus sign
		n = -n;
	}
	
	while (n > 0) {
		n /= 10;
		count++;
	}
	return (count);
}

static void get_colored_name(const FileInfo *file, char *buffer, size_t buffer_size) {
	const char *color = RESET;
	
	if (S_ISDIR(file->stat.st_mode))        color = BLUE;
	else if (S_ISFIFO(file->stat.st_mode))  color = CYAN;
	else if (S_ISLNK(file->stat.st_mode))   color = CYAN;
	else if (file->stat.st_mode & S_IXUSR)  color = GREEN;

	snprintf(buffer, buffer_size, "%s%s%s", color, file->name, RESET);
}

static int get_terminal_width(void) {
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
		return (w.ws_col);
	}
	return (80); // Default width
}

static int get_display_width(const char *str) {
	int width = 0;
	int in_escape = 0;
	
	for (int i = 0; str[i]; i++) {
		if (str[i] == '\x1B') {
			in_escape = 1;
		} else if (in_escape && str[i] == 'm') {
			in_escape = 0;
		} else if (!in_escape) {
			width++;
		}
	}
	return (width);
}

static DisplayData *create_display_data(DirectoryInfo directory) {
	DisplayData *data = malloc(sizeof(DisplayData));
	if (!data) {
		return (NULL);
	}
	
	data->count = directory.files_count;
	data->display_names = calloc(data->count, sizeof(char *));
	data->widths = calloc(data->count, sizeof(int));
	
	if (!data->display_names || !data->widths) {
		free(data->display_names);
		free(data->widths);
		free(data);
		return (NULL);
	}
	
	// Prepare display names and calculate widths
	for (size_t i = 0; i < data->count; i++) {
		data->display_names[i] = malloc(256);
		if (!data->display_names[i]) {
			// Cleanup on failure
			for (size_t j = 0; j < i; j++) {
				free(data->display_names[j]);
			}
			free(data->display_names);
			free(data->widths);
			free(data);
			return (NULL);
		}
		
		get_colored_name(directory.files[i], data->display_names[i], 256);
		data->widths[i] = get_display_width(data->display_names[i]);
	}
	
	return (data);
}

static void free_display_data(DisplayData *data) {
	if (!data) return;
	
	if (data->display_names) {
		for (size_t i = 0; i < data->count; i++) {
			free(data->display_names[i]);
		}
		free(data->display_names);
	}
	free(data->widths);
	free(data);
}

static LayoutInfo calculate_layout(DisplayData *data, int cols, int term_width) {
	LayoutInfo layout = {0};
	layout.cols = cols;
	layout.rows = (data->count + cols - 1) / cols;
	layout.col_widths = malloc(sizeof(int) * cols);
	layout.valid = 1;
	
	if (!layout.col_widths) {
		layout.valid = 0;
		return (layout);
	}
	
	// Calculate column widths
	for (int col = 0; col < cols; col++) {
		int max_width = 0;
		
		for (int row = 0; row < layout.rows; row++) {
			int index = col * layout.rows + row;
			if (index < (int)data->count) {
				if (data->widths[index] > max_width) {
					max_width = data->widths[index];
				}
			}
		}
		
		layout.col_widths[col] = max_width;
		layout.total_width += max_width;
		
		// Add spacing between columns
		if (col < cols - 1) {
			layout.total_width += 2;
		}
	}
	
	layout.valid = (layout.total_width <= term_width);
	return (layout);
}

static LayoutInfo find_best_layout(DisplayData *data, int term_width) {
	LayoutInfo best_layout = {1, (int)data->count, NULL, 0, 1};
	
	for (int cols = 1; cols <= (int)data->count; cols++) {
		LayoutInfo current = calculate_layout(data, cols, term_width);
		
		if (current.valid && 
			(current.rows < best_layout.rows || 
			 (current.rows == best_layout.rows && current.cols > best_layout.cols))) {
			
			free(best_layout.col_widths);
			best_layout = current;
		} else {
			free(current.col_widths);
		}
		
		if (current.rows == 1) {
			break;
		}
	}
	
	return (best_layout);
}

static void print_layout(DisplayData *data, LayoutInfo layout) {
	for (int row = 0; row < layout.rows; row++) {
		for (int col = 0; col < layout.cols; col++) {
			int index = col * layout.rows + row;
			if (index >= (int)data->count) break;
			
			printf("%s", data->display_names[index]);
			
			// Add padding for next column
			if (col < layout.cols - 1) {
				int next_index = (col + 1) * layout.rows + row;
				if (next_index < (int)data->count) {
					int padding = layout.col_widths[col] - data->widths[index] + 2;
					printf("%*s", padding, "");
				}
			}
		}
		printf("\n");
	}
}

static void print_in_columns(DirectoryInfo directory) {
	if (directory.files_count == 0) {
		return;
	}
	
	DisplayData *data = create_display_data(directory);
	if (!data) {
		fprintf(stderr, "Failed to create display data\n");
		return;
	}
	
	int term_width = get_terminal_width();
	LayoutInfo best_layout = find_best_layout(data, term_width);
	
	print_layout(data, best_layout);
	
	free(best_layout.col_widths);
	free_display_data(data);
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
	
	const time_t six_months = 180 * 24 * 60 * 60;
	int is_recent = (file_time >= now - six_months) && 
					(file_time <= now + six_months) &&
					(file_tm->tm_year == now_tm->tm_year);

	if (is_recent) {
		snprintf(buffer, 13, "%s %2d %02d:%02d",
				months[file_tm->tm_mon], file_tm->tm_mday,
				file_tm->tm_hour, file_tm->tm_min);
	} else {
		snprintf(buffer, 13, "%s %2d  %04d",
				months[file_tm->tm_mon], file_tm->tm_mday,
				file_tm->tm_year + 1900);
	}
}

static char get_file_type_char(mode_t mode) {
	if (S_ISDIR(mode))       return ('d');
	if (S_ISCHR(mode))       return ('c');
	if (S_ISBLK(mode))       return ('b');
	if (S_ISFIFO(mode))      return ('p');
	if (S_ISLNK(mode))       return ('l');
	if (S_ISSOCK(mode))      return ('s');
	return ('-');
}

static void format_permissions(FileInfo *file, char *buffer) {
	mode_t mode = file->stat.st_mode;
	
	buffer[0] = get_file_type_char(mode);
	
	// User permissions
	buffer[1] = (mode & S_IRUSR) ? 'r' : '-';
	buffer[2] = (mode & S_IWUSR) ? 'w' : '-';
	buffer[3] = (mode & S_IXUSR) ? 
		((mode & S_ISUID) ? 's' : 'x') : 
		((mode & S_ISUID) ? 'S' : '-');
	
	// Group permissions
	buffer[4] = (mode & S_IRGRP) ? 'r' : '-';
	buffer[5] = (mode & S_IWGRP) ? 'w' : '-';
	buffer[6] = (mode & S_IXGRP) ?
		((mode & S_ISGID) ? 's' : 'x') :
		((mode & S_ISGID) ? 'S' : '-');
	
	// Other permissions
	buffer[7] = (mode & S_IROTH) ? 'r' : '-';
	buffer[8] = (mode & S_IWOTH) ? 'w' : '-';
	buffer[9] = (mode & S_IXOTH) ?
		((mode & S_ISVTX) ? 't' : 'x') :
		((mode & S_ISVTX) ? 'T' : '-');

	buffer[10] = (listxattr(file->name, NULL, 0) > 0) ? '@' : ' ';
	buffer[11] = '\0';
}

static StringBuffer *create_string_buffer(size_t initial_size) {
	StringBuffer *sb = malloc(sizeof(StringBuffer));
	if (!sb) {
		return (NULL);
	}
	
	sb->buffer = malloc(initial_size);
	if (!sb->buffer) {
		free(sb);
		return (NULL);
	}
	
	sb->size = initial_size;
	sb->pos = 0;
	return (sb);
}

static int append_to_buffer(StringBuffer *sb, const char *format, ...) {
	va_list args;
	va_start(args, format);
	
	int needed = vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	if (sb->pos + needed + 1 > sb->size) {
		size_t new_size = sb->size * 2;
		while (new_size < sb->pos + needed + 1) {
			new_size *= 2;
		}
		
		char *new_buffer = realloc(sb->buffer, new_size);
		if (!new_buffer) {
			return (-1);
		}
		
		sb->buffer = new_buffer;
		sb->size = new_size;
	}
	
	va_start(args, format);
	int written = vsnprintf(sb->buffer + sb->pos, sb->size - sb->pos, format, args);
	va_end(args);
	
	sb->pos += written;
	return (written);
}

static void free_string_buffer(StringBuffer *sb) {
	if (!sb) {
		return;
	}
	free(sb->buffer);
	free(sb);
}

static void print_long_format(DirectoryInfo directory) {
	StringBuffer *sb = create_string_buffer(4096);
	if (!sb) {
		return;
	}

	for (size_t i = 0; i < directory.files_count; i++) {
		FileInfo *file = directory.files[i];
		char date_buf[64], perm_buf[64], name_buf[256];
		
		get_colored_name(file, name_buf, sizeof(name_buf));
		format_date(file, date_buf);
		format_permissions(file, perm_buf);

		struct passwd *pwd = getpwuid(file->stat.st_uid);
		struct group *grp = getgrgid(file->stat.st_gid);
		const char *username = pwd ? pwd->pw_name : "unknown";
		const char *groupname = grp ? grp->gr_name : "unknown";

		const char *link_indicator = file->link_name ? " -> " : "";
		const char *link_target = file->link_name ? file->link_name : "";

		append_to_buffer(sb, "%s %*ld %-*s %-*s %*lu %s %s%s%s\n",
			perm_buf, directory.widths.blocks, file->stat.st_nlink,
			directory.widths.user, username,
			directory.widths.group, groupname,
			directory.widths.size, file->stat.st_size, date_buf, name_buf,
			link_indicator, link_target);
	}

	fwrite(sb->buffer, 1, sb->pos, stdout);
	free_string_buffer(sb);
}

static void print_directory(DirectoryInfo directory, const char *path) {
	if (options & RECURSE) {
		printf("%s:\n", path);
	}
	
	if (options & LIST) {
		printf("total %lu\n", directory.total_blocks);
		print_long_format(directory);
	} else {
		print_in_columns(directory);
	}
}

static void free_file(FileInfo *file) {
	if (!file) {
		return;
	}
	free(file->link_name);
	free(file->name);
	free(file);
}

static void free_files(FileInfo **files, size_t count) {
	if (!files) {
		return;
	}
	
	for (size_t i = 0; i < count; i++) {
		free_file(files[i]);
	}
	free(files);
}

static int should_skip_file(const char *name, ShowType show_type) {
	int is_hidden = (name[0] == '.');
	int is_special_dir = (!ft_strcmp(name, ".") || !ft_strcmp(name, ".."));
	
	switch (show_type) {
		case SHOW_NORMAL:      return (is_hidden);
		case SHOW_ALMOST_ALL:  return (is_special_dir);
		case SHOW_ALL:         return (0);
		default:               return (is_hidden);
	}
}

static FileInfo *create_file_info(const char *name, const char *full_path, struct stat *st) {
	FileInfo *file = malloc(sizeof(FileInfo));
	if (!file) {
		return ((NULL));
	}
	
	file->name = ft_strdup(name);
	if (!file->name) {
		free(file);
		return (NULL);
	}
	
	file->link_name = NULL;
	file->stat = *st;
	
	// Handle symlinks
	if (S_ISLNK(st->st_mode)) {
		char *link_target = malloc(1024);
		if (link_target) {
			ssize_t len = readlink(full_path, link_target, 1023);
			if (len > 0) {
				link_target[len] = '\0';
				file->link_name = link_target;
			} else {
				free(link_target);
			}
		}
	}
	
	return (file);
}

static void update_directory_stats(DirectoryInfo *data, FileInfo *file) {
	struct passwd *pwd = getpwuid(file->stat.st_uid);
	struct group *grp = getgrgid(file->stat.st_gid);
	const char *user = pwd ? pwd->pw_name : "unknown";
	const char *group = grp ? grp->gr_name : "unknown";

	data->total_blocks += file->stat.st_blocks / 2;
	data->widths.blocks = MAX(data->widths.blocks, number_len(file->stat.st_nlink));
	data->widths.size = MAX(data->widths.size, number_len(file->stat.st_size));
	data->widths.user = MAX(data->widths.user, ft_strlen(user));
	data->widths.group = MAX(data->widths.group, ft_strlen(group));
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
		closedir(dir);
		fprintf(stderr, "malloc failed\n");
		return (data);
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (should_skip_file(entry->d_name, show_type)) {
			continue;
		}

		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		struct stat st;
		if (lstat(full_path, &st) != 0) {
			continue;
		}

		FileInfo *file = create_file_info(entry->d_name, full_path, &st);
		if (!file) {
			free_files(data.files, data.files_count);
			closedir(dir);
			fprintf(stderr, "Failed to create file info\n");
			return ((DirectoryInfo){0});
		}

		// Resize array if needed
		if (data.files_count >= capacity) {
			capacity *= 2;
			FileInfo **new_files = realloc(data.files, capacity * sizeof(FileInfo *));
			if (!new_files) {
				free_file(file);
				free_files(data.files, data.files_count);
				closedir(dir);
				fprintf(stderr, "realloc failed\n");
				return ((DirectoryInfo){0});
			}
			data.files = new_files;
		}

		data.files[data.files_count++] = file;
		update_directory_stats(&data, file);
	}

	closedir(dir);
	return (data);
}

static void process_directory(char *path) {
	DirectoryInfo data = read_directory(path);
	if (!data.files) {
		return;
	}
	
	// Sort files based on current sort_type
	switch (sort_type) {
		case SORT_NONE: break;
		case SORT_TIME: quicksort(data.files, data.files_count, sizeof(FileInfo *), compare_file_mtime); break;
		case SORT_SIZE: quicksort(data.files, data.files_count, sizeof(FileInfo *), compare_file_size); break;
		case SORT_NAME:
		default: quicksort(data.files, data.files_count, sizeof(FileInfo *), compare_file_name); break;
	}

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
		}
	}
	
	free_files(data.files, data.files_count);
}

static int process_options(int ac, char **av) {
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

static int process_names(int ac, char **av, char ***names) {
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
			if (i != 0) printf("\n");
			process_directory(names[i]);
		}
	} else {
		process_directory(".");
	}
	
	free(names);
	return (EXIT_SUCCESS);
}