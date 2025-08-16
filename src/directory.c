#include "ls.h"

extern Options options;
extern SortType sort_type;
extern ShowType show_type;

# define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

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

int should_skip_file(const char *name, ShowType show_type) {
	int is_hidden = (name[0] == '.');
	int is_special_dir = (!ft_strcmp(name, ".") || !ft_strcmp(name, ".."));
	
	switch (show_type) {
		case SHOW_NORMAL:      return (is_hidden);
		case SHOW_ALMOST_ALL:  return (is_special_dir);
		case SHOW_ALL:         return (0);
		default:               return (is_hidden);
	}
}

FileInfo *create_file_info(const char *name, const char *full_path, struct stat *st) {
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

void update_directory_stats(DirectoryInfo *data, FileInfo *file) {
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

DirectoryInfo read_directory(char *path) {
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

static void print_directory(DirectoryInfo directory, const char *path) {
	if (options & RECURSE) {
		printf("%s:\n", path);
	}
	
	if ((options & LIST) || (options & LIST_GROUP_ONLY)) {
		printf("total %lu\n", directory.total_blocks);
		print_list_formatted(directory);
	} else {
		print_formatted(directory);
	}
}

void process_directory(char *path) {
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

    if (options & REVERSE) {
		reverse(data.files, data.files_count, sizeof(FileInfo *));
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

void free_file(FileInfo *file) {
	if (!file) {
		return;
	}
	free(file->link_name);
	free(file->name);
	free(file);
}

void free_files(FileInfo **files, size_t count) {
	if (!files) {
		return;
	}
	
	for (size_t i = 0; i < count; i++) {
		free_file(files[i]);
	}
	free(files);
}