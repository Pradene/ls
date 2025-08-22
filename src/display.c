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

static int get_terminal_width(void) {
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
		return (w.ws_col);
	}
	return (80);
}

static int get_display_width(const char *str) {
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

static const char *get_color_by_mode(mode_t mode) {
	if (S_ISDIR(mode))   return BLUE;
	if (S_ISFIFO(mode))  return CYAN;
	if (S_ISLNK(mode))   return CYAN;
	if (mode & S_IXUSR)  return GREEN;
	return RESET;
}

static void print_colored_name(const FileInfo *file) {
	ft_printf("%s%s%s", 
		get_color_by_mode(file->stat.st_mode),
		file->name,
		RESET);
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

static void print_permissions(FileInfo *file) {
	mode_t mode = file->stat.st_mode;
	
	ft_printf("%c", get_file_type_char(mode));
	
	// User permissions
	ft_printf("%c", (mode & S_IRUSR) ? 'r' : '-');
	ft_printf("%c", (mode & S_IWUSR) ? 'w' : '-');
	ft_printf("%c", (mode & S_IXUSR) ? 
		((mode & S_ISUID) ? 's' : 'x') : 
		((mode & S_ISUID) ? 'S' : '-'));
	
	// Group permissions
	ft_printf("%c", (mode & S_IRGRP) ? 'r' : '-');
	ft_printf("%c", (mode & S_IWGRP) ? 'w' : '-');
	ft_printf("%c", (mode & S_IXGRP) ?
		((mode & S_ISGID) ? 's' : 'x') :
		((mode & S_ISGID) ? 'S' : '-'));
	
	// Other permissions
	ft_printf("%c", (mode & S_IROTH) ? 'r' : '-');
	ft_printf("%c", (mode & S_IWOTH) ? 'w' : '-');
	ft_printf("%c", (mode & S_IXOTH) ?
		((mode & S_ISVTX) ? 't' : 'x') :
		((mode & S_ISVTX) ? 'T' : '-'));

	if (listxattr(file->name, NULL, 0) > 0) {
		ft_printf("@");
	}
}

static void print_date(const FileInfo *file) {
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
		(file_tm->tm_year == now_tm->tm_year)) {
		ft_printf("%s %2d %02d:%02d",
			months[file_tm->tm_mon], file_tm->tm_mday,
			file_tm->tm_hour, file_tm->tm_min);
	} else {
		ft_printf("%s %2d  %04d",
			months[file_tm->tm_mon], file_tm->tm_mday,
			file_tm->tm_year + 1900);
	}
}

static void print_colored_link_target(const char *link_path) {
	const char *color = RESET;
	struct stat st;
	
	if (lstat(link_path, &st) == 0) {
		color = get_color_by_mode(st.st_mode);
	}
	ft_printf("%s%s%s", color, link_path, RESET);
}

static int create_display_directory(DirectoryInfo *directory, DisplayArray *display_array) {
	display_array->items = NULL;
	display_array->count = 0;
	display_array->capacity = 0;
	
	if (!ft_da_resize(display_array, directory->files.count)) {
		return (-1);
	}
	
	for (size_t i = 0; i < directory->files.count; i++) {
		display_array->items[i].display_name = malloc(256);
		if (!display_array->items[i].display_name) {
			for (size_t j = 0; j < i; j++) {
				free(display_array->items[j].display_name);
			}
			ft_da_free(*display_array);
			return (-1);
		}
		
		char *buf = display_array->items[i].display_name;
		int len = 0;
		const char *color = get_color_by_mode(directory->files.items[i].stat.st_mode);
		
		while (*color && len < 255) {
			buf[len++] = *color++;
		}
		
		const char *name = directory->files.items[i].name;
		while (*name && len < 255) {
			buf[len++] = *name++;
		}
		
		const char *reset = RESET;
		while (*reset && len < 255) {
			buf[len++] = *reset++;
		}
		buf[len] = '\0';
		
		display_array->items[i].width = get_display_width(display_array->items[i].display_name);
	}
	
	return (0);
}

static void free_display_array(DisplayArray *display_array) {
	if (display_array->items) {
		for (size_t i = 0; i < display_array->count; i++) {
			free(display_array->items[i].display_name);
		}
		ft_da_free(*display_array);
	}
}

static LayoutInfo calculate_layout(DisplayArray *display_array, int cols, int term_width) {
	LayoutInfo layout = {0};
	layout.cols = cols;
	layout.rows = ((int)display_array->count + cols - 1) / cols;
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
			if (index < (int)display_array->count) {
				if (display_array->items[index].width > max_width) {
					max_width = display_array->items[index].width;
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

static LayoutInfo find_best_layout(DisplayArray *display_array, int term_width) {
	int directory_count = (int)display_array->count;
	LayoutInfo best_layout = {1, directory_count, NULL, 0, 1};
	
	for (int cols = 1; cols <= directory_count; cols++) {
		LayoutInfo current = calculate_layout(display_array, cols, term_width);
		
		if (current.valid && 
			(current.rows < best_layout.rows || 
			(current.rows == best_layout.rows && current.cols > best_layout.cols))) {
			
			free(best_layout.col_widths);
			best_layout = current;
		} else {
			free(current.col_widths);
		}
		
		if (current.rows == 1) {
			break ;
		}
	}
	
	return (best_layout);
}

static void print_layout(DisplayArray *display_array, LayoutInfo layout) {
	for (int row = 0; row < layout.rows; row++) {
		for (int col = 0; col < layout.cols; col++) {
			int index = col * layout.rows + row;
			if (index >= (int)display_array->count) break ;
			
			ft_printf("%s", display_array->items[index].display_name);
			
			if (col < layout.cols - 1) {
				int next_index = (col + 1) * layout.rows + row;
				if (next_index < (int)display_array->count) {
					int padding = layout.col_widths[col] - display_array->items[index].width + 2;
					ft_printf("%*s", padding, "");
				}
			}
		}
		ft_printf("\n");
	}
}

static ColumnWidths get_list_format(DirectoryInfo *directory) {
	ColumnWidths widths = {0};

	for (size_t i = 0; i < directory->files.count; ++i) {
		FileInfo *file = &directory->files.items[i];

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
	for (size_t i = 0; i < directory->files.count; ++i) {
		FileInfo *file = &directory->files.items[i];
		blocks += file->stat.st_blocks / 2; 
	}
	return (blocks);
}

void print_formatted(DirectoryInfo *directory) {
	if (directory->files.count == 0) {
		return;
	}
	
	DisplayArray display_array;
	if (create_display_directory(directory, &display_array) != 0) {
		fprintf(stderr, "Failed to create display directory\n");
		return;
	}
	
	int term_width = get_terminal_width();
	LayoutInfo best_layout = find_best_layout(&display_array, term_width);
	
	print_layout(&display_array, best_layout);
	
	free(best_layout.col_widths);
	free_display_array(&display_array);
}

void print_list_formatted(DirectoryInfo *directory) {
	ft_printf("total %zu\n", get_total_blocks(directory));
	ColumnWidths widths = get_list_format(directory);
	
	for (size_t i = 0; i < directory->files.count; i++) {
		FileInfo *file = &directory->files.items[i];

		struct passwd *pwd = getpwuid(file->stat.st_uid);
		struct group *grp = getgrgid(file->stat.st_gid);
		const char *username = pwd ? pwd->pw_name : "unknown";
		const char *groupname = grp ? grp->gr_name : "unknown";

		print_permissions(file);
		ft_printf(" ");
		
		ft_printf("%*zu ", widths.nlink, file->stat.st_nlink);
		
		if (!(options & LIST_GROUP_ONLY)) {
			ft_printf("%*s ", widths.user, username);
		}
		
		ft_printf("%*s ", widths.group, groupname);
		
		ft_printf("%*zu ", widths.size, file->stat.st_size);
		
		print_date(file);
		ft_printf(" ");
		
		print_colored_name(file);
		
		if (file->link) {
			ft_printf(" -> ");
			print_colored_link_target(file->link);
		}
		
		ft_printf("\n");
	}
}
