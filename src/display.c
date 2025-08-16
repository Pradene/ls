#include "ls.h"

extern Options options;

void get_colored_name(const FileInfo *file, char *buffer, size_t buffer_size) {
	const char *color = RESET;
	
	if (S_ISDIR(file->stat.st_mode))        color = BLUE;
	else if (S_ISFIFO(file->stat.st_mode))  color = CYAN;
	else if (S_ISLNK(file->stat.st_mode))   color = CYAN;
	else if (file->stat.st_mode & S_IXUSR)  color = GREEN;

	snprintf(buffer, buffer_size, "%s%s%s", color, file->name, RESET);
}

int get_terminal_width(void) {
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
		return (w.ws_col);
	}
	return (80); // Default width
}

int get_display_width(const char *str) {
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
	data->display_names = malloc(data->count * sizeof(char *));
	data->widths = malloc(data->count * sizeof(int));
	
	if (!data->display_names || !data->widths) {
		free(data->display_names);
		free(data->widths);
		free(data);
		return (NULL);
	}
	
	for (size_t i = 0; i < data->count; i++) {
		data->display_names[i] = malloc(256);
		if (!data->display_names[i]) {
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

void print_formatted(DirectoryInfo directory) {
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

void format_date(const FileInfo *file, char *buffer) {
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

	buffer[10] = (listxattr(file->name, NULL, 0) > 0) ? '@' : ' ';
	buffer[11] = '\0';
}

void print_list_formatted(DirectoryInfo directory) {
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

		if (options & LIST_GROUP_ONLY) {
			// -g option: show long format but without owner
			append_to_buffer(sb, "%s %*ld %-*s %*lu %s %s%s%s\n",
				perm_buf, directory.widths.blocks, file->stat.st_nlink,
				directory.widths.group, groupname,
				directory.widths.size, file->stat.st_size, date_buf, name_buf,
				link_indicator, link_target);
		} else {
			// -l option: show full long format with owner
			append_to_buffer(sb, "%s %*ld %-*s %-*s %*lu %s %s%s%s\n",
				perm_buf, directory.widths.blocks, file->stat.st_nlink,
				directory.widths.user, username,
				directory.widths.group, groupname,
				directory.widths.size, file->stat.st_size, date_buf, name_buf,
				link_indicator, link_target);
		}
	}

	fwrite(sb->buffer, 1, sb->pos, stdout);
	free_string_buffer(sb);
}