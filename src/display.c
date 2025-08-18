#include "ls.h"

extern Options options;

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

static inline int number_len(long long n) {
	if (n == 0) {
		return (1);
	}
	
	int count = 0;
	if (n < 0) {
		count = 1;
		n = -n;
	}
	
	while (n > 0) {
		n /= 10;
		count++;
	}
	return (count);
}

static const char *get_color_by_mode(mode_t mode) {
	if (S_ISDIR(mode))   return BLUE;
	if (S_ISFIFO(mode))  return CYAN;
	if (S_ISLNK(mode))   return CYAN;
	if (mode & S_IXUSR)  return GREEN;
	return RESET;
}

static void get_colored_name(const FileInfo *file, char *buffer, size_t buffer_size) {
	snprintf(
		buffer,
		buffer_size,
		"%s%s%s",
		get_color_by_mode(file->stat.st_mode),
		file->name,
		RESET
	);
}

int get_terminal_width(void) {
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
		return (w.ws_col);
	}
	return (80);
}

int get_display_width(const char *str) {
	int width = 0;
	bool in_escape = false;
	
	for (int i = 0; str[i]; i++) {
		if (str[i] == '\x1B') {
			in_escape = true;
		} else if (in_escape == true && str[i] == 'm') {
			in_escape = false;
		} else if (in_escape == false) {
			width++;
		}
	}
	return (width);
}

static void free_display_info(void *item) {
	DisplayInfo *display_info = (DisplayInfo *)item;
	free(display_info->display_name);
	free(display_info);
}

static DynamicArray *create_display_directory(DirectoryInfo *directory) {
	DynamicArray *display_array = da_create(da_size(directory->files));
	if (!display_array) {
		return (NULL);
	}
	
	for (size_t i = 0; i < da_size(directory->files); i++) {
		DisplayInfo *item = malloc(sizeof(DisplayInfo));
		if (!item) {
			da_destroy(display_array, free_display_info);
			return (NULL);
		}
		
		item->display_name = malloc(256);
		if (!item->display_name) {
			free(item);
			da_destroy(display_array, free_display_info);
			return (NULL);
		}
		
		get_colored_name(da_get(directory->files, i), item->display_name, 256);
		item->width = get_display_width(item->display_name);
		
		if (!da_push(display_array, item)) {
			free_display_info(item);
			da_destroy(display_array, free_display_info);
			return (NULL);
		}
	}
	
	return (display_array);
}

static LayoutInfo calculate_layout(DynamicArray *directory, int cols, int term_width) {
	LayoutInfo layout = {0};
	layout.cols = cols;
	layout.rows = ((int)da_size(directory) + cols - 1) / cols;
	layout.col_widths = malloc(sizeof(int) * cols);
	layout.valid = 1;
	
	if (layout.col_widths == NULL) {
		layout.valid = 0;
		return (layout);
	}
	
	for (int col = 0; col < cols; col++) {
		int max_width = 0;
		
		for (int row = 0; row < layout.rows; row++) {
			int index = col * layout.rows + row;
			if (index < (int)da_size(directory)) {
				DisplayInfo *item = (DisplayInfo *)da_get(directory, index);
				if (item && item->width > max_width) {
					max_width = item->width;
				}
			}
		}
		
		layout.col_widths[col] = max_width;
		layout.total_width += max_width;
		
		if (col < cols - 1) {
			layout.total_width += 2;
		}
	}
	
	layout.valid = (layout.total_width <= term_width);
	return (layout);
}

static LayoutInfo find_best_layout(DynamicArray *directory, int term_width) {
	int directory_count = (int)da_size(directory);
	LayoutInfo best_layout = {1, directory_count, NULL, 0, 1};
	
	for (int cols = 1; cols <= directory_count; cols++) {
		LayoutInfo current = calculate_layout(directory, cols, term_width);
		
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

static void print_layout(DynamicArray *directory, LayoutInfo layout) {
	for (int row = 0; row < layout.rows; row++) {
		for (int col = 0; col < layout.cols; col++) {
			int index = col * layout.rows + row;
			if (index >= (int)da_size(directory)) break;
			
			DisplayInfo *item = (DisplayInfo *)da_get(directory, index);
			if (!item) continue;
			
			ft_printf("%s", item->display_name);
			
			if (col < layout.cols - 1) {
				int next_index = (col + 1) * layout.rows + row;
				if (next_index < (int)da_size(directory)) {
					int padding = layout.col_widths[col] - item->width + 2;
					ft_printf("%*s", padding, "");
				}
			}
		}
		ft_printf("\n");
	}
}

void print_formatted(DirectoryInfo *directory) {
	if (da_size(directory->files) == 0) {
		return;
	}
	
	DynamicArray *display_directory = create_display_directory(directory);
	if (display_directory == NULL) {
		fprintf(stderr, "Failed to create display directory\n");
		return;
	}
	
	int term_width = get_terminal_width();
	LayoutInfo best_layout = find_best_layout(display_directory, term_width);
	
	print_layout(display_directory, best_layout);
	
	free(best_layout.col_widths);
	da_destroy(display_directory, free_display_info);
}

void format_date(const FileInfo *file, char *buffer) {
	time_t now = time(NULL);
	time_t file_time = file->stat.st_mtime;
	if (options & ACCESS_TIME) {
		file_time = file->stat.st_atime;
	}
	struct tm *now_tm = localtime(&now);
	struct tm *file_tm = localtime(&file_time);
	
	const char *months[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	
	const time_t six_months = 180 * 24 * 60 * 60;

	if ((file_time >= now - six_months) &&
		(file_time <= now + six_months) &&
		(file_tm->tm_year == now_tm->tm_year)
	) {
		snprintf(
			buffer, 13, "%s %2d %02d:%02d",
			months[file_tm->tm_mon], file_tm->tm_mday,
			file_tm->tm_hour, file_tm->tm_min
		);
	} else {
		snprintf(
			buffer, 13, "%s %2d  %04d",
			months[file_tm->tm_mon], file_tm->tm_mday,
			file_tm->tm_year + 1900
		);
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

void format_permissions(FileInfo *file, char *buffer) {
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

	buffer[10] = (listxattr(file->name, NULL, 0) > 0) ? '@' : '\0';
	buffer[11] = '\0';
}

ColumnWidths get_list_format(DirectoryInfo *directory) {
	ColumnWidths widths = {0};

	for (size_t i = 0; i < da_size(directory->files); ++i) {
		FileInfo *file = da_get(directory->files, i);

		struct passwd *usr = getpwuid(file->stat.st_uid);
		const char *user = usr ? usr->pw_name : "unknown";

		struct group *grp = getgrgid(file->stat.st_gid);
		const char *group = grp ? grp->gr_name : "unknown";
		
		widths.nlink = MAX(widths.nlink, number_len(file->stat.st_nlink));
		widths.size  = MAX(widths.size, number_len(file->stat.st_size));
		widths.user  = MAX(widths.user, ft_strlen(user));
		widths.group = MAX(widths.group, ft_strlen(group));
	}

	return (widths);
}

static size_t get_total_blocks(DirectoryInfo *directory) {
	size_t blocks = 0;
	for (size_t i = 0; i < da_size(directory->files); ++i) {
		FileInfo *file = (FileInfo *)da_get(directory->files, i);
		if (file == NULL) continue;
		blocks += file->stat.st_blocks / 2; 
	}
	return (blocks);
}

void print_list_formatted(DirectoryInfo *directory) {
	ft_printf("total %z\n", get_total_blocks(directory));
	ColumnWidths widths = get_list_format(directory);
	
	for (size_t i = 0; i < da_size(directory->files); i++) {
		FileInfo *file = da_get(directory->files, i);
		if (file == NULL) continue;
		
		char date_buf[64], perm_buf[64], name_buf[256];
		
		get_colored_name(file, name_buf, sizeof(name_buf));
		format_date(file, date_buf);
		format_permissions(file, perm_buf);

		struct passwd *pwd = getpwuid(file->stat.st_uid);
		struct group *grp = getgrgid(file->stat.st_gid);
		const char *username = pwd ? pwd->pw_name : "unknown";
		const char *groupname = grp ? grp->gr_name : "unknown";

		const char *link_indicator = file->link_name ? "->" : "";
		const char *link_target = file->link_name ? file->link_name : "";
		char colored_target[256] = {0};

		if (file->link_name) {
			const char *color = RESET;
			
			struct stat st;
			if (lstat(file->link_name, &st) == 0) {
				color = get_color_by_mode(st.st_mode);
			}
			snprintf(colored_target, sizeof(colored_target), "%s%s%s", color, file->link_name, RESET);
			link_target = colored_target;
		}

		if (options & LIST_GROUP_ONLY) {
			ft_printf("%s %*z %-*s %*z %s %s %s %s\n",
				perm_buf, widths.nlink, file->stat.st_nlink,
				widths.group, groupname,
				widths.size, file->stat.st_size,
				date_buf, name_buf,
				link_indicator, link_target
			);
		} else {
			ft_printf("%s %*z %-*s %-*s %*z %s %s %s %s\n",
				perm_buf, widths.nlink, file->stat.st_nlink,
				widths.user, username,
				widths.group, groupname,
				widths.size, file->stat.st_size,
				date_buf, name_buf,
				link_indicator, link_target
			);
		}
	}
}