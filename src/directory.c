#include "ls.h"

extern Options options;
extern SortType sort_type;
extern ShowType show_type;

bool should_skip_file(const char *name, ShowType show_type) {
	bool is_hidden = (name[0] == '.');
	bool is_special_dir = (!ft_strcmp(name, ".") || !ft_strcmp(name, ".."));
	
	switch (show_type) {
		case SHOW_ALL:         return (0);
		case SHOW_ALMOST_ALL:  return (is_special_dir);
		case SHOW_VISIBLE:
		default:               return (is_hidden);
	}
}

FileInfo *create_file_info(const char *name, const char *full_path, struct stat *st) {
	FileInfo *file = malloc(sizeof(FileInfo));
	if (file == NULL) {
		return (NULL);
	}
	
	file->name = ft_strdup(name);
	if (file->name == NULL) {
		free(file);
		return (NULL);
	}
	
	file->link_name = NULL;
	file->stat = *st;
	
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

DirectoryInfo read_directory(char *path) {
	DirectoryInfo data = {0};

	data.path = ft_strdup(path);
	if (data.path == NULL) {
		return ((DirectoryInfo){0});
	}

	size_t capacity = 16;
	data.files = malloc(capacity * sizeof(FileInfo *));
	if (data.files == NULL) {
		fprintf(stderr, "malloc failed\n");
		return ((DirectoryInfo){0});
	}

	DIR *dir = opendir(path);
	if (dir == NULL) {
		free(data.path);
		free(data.files);
		fprintf(stderr, "opendir failed\n");
		return ((DirectoryInfo){0});
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (should_skip_file(entry->d_name, show_type) == true) {
			continue;
		}

		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		struct stat st;
		if (lstat(full_path, &st) != 0) {
			continue;
		}

		FileInfo *file = create_file_info(entry->d_name, full_path, &st);
		if (file == NULL) {
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
	}

	closedir(dir);
	return (data);
}

static size_t get_total_blocks(DirectoryInfo *data) {
	size_t blocks = 0;
	for (size_t i = 0; i < data->files_count; ++i) {
		blocks += data->files[i]->stat.st_blocks / 2; 
	}
	return (blocks);
}

static void print_directory(DirectoryInfo *data) {
	if (options & RECURSE) {
		printf("%s:\n", data->path);
	}
	
	if ((options & LIST) || (options & LIST_GROUP_ONLY)) {
		printf("total %lu\n", get_total_blocks(data));
		print_list_formatted(*data);
	} else {
		print_formatted(*data);
	}
}

void sort_directory(DirectoryInfo *data) {
	switch (sort_type) {
		case SORT_NONE: break;
		case SORT_TIME: quicksort(data->files, data->files_count, sizeof(FileInfo *), compare_file_mtime); break;
		case SORT_SIZE: quicksort(data->files, data->files_count, sizeof(FileInfo *), compare_file_size); break;
		case SORT_NAME:
		default: quicksort(data->files, data->files_count, sizeof(FileInfo *), compare_file_name); break;
	}

	if (options & REVERSE) {
		reverse(data->files, data->files_count, sizeof(FileInfo *));
	}
}

void process_directory(char *path) {
	DirectoryInfo data = read_directory(path);
	if (data.files == NULL) {
		return;
	}

	sort_directory(&data);
	print_directory(&data);
	
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
	free(data.path);
}

void free_file(FileInfo *file) {
	if (file == NULL) return;
	free(file->link_name);
	free(file->name);
	free(file);
}

void free_files(FileInfo **files, size_t count) {
	if (files == NULL) return;
	for (size_t i = 0; i < count; i++) free_file(files[i]);
	free(files);
}