#include "ls.h"

extern Options options;
extern SortType sort_type;
extern ShowType show_type;

static void free_file(void *ptr) {
	FileInfo *file = (FileInfo *)ptr;
	if (!file) return;
	free(file->link_name);
	free(file->name);
	free(file);
}

static void free_directory(DirectoryInfo *data) {
	for (size_t i = 0; i < data->files_count; i++) free_file(data->files[i]);
	free(data->files);
	free(data->path);
	data->files = NULL;
	data->path = NULL;
}

static bool is_file_hidden(const char *name) {
	return (name[0] == '.');
}

static bool is_file_special(const char *name) {
	return (!ft_strcmp(name, ".") || !ft_strcmp(name, ".."));
}

static bool should_skip_file(const char *name) {
	switch (show_type) {
		case SHOW_ALL:         return (0);
		case SHOW_ALMOST_ALL:  return (is_file_special(name));
		case SHOW_VISIBLE:
		default:               return (is_file_hidden(name));
	}
}

static char *build_path(const char *dir_path, const char *filename) {
	size_t dir_len = ft_strlen(dir_path);
	size_t name_len = ft_strlen(filename);
	size_t total_len = dir_len + name_len + 2; // + 2 for '/' and '\0'
	
	char *full_path = malloc(total_len);
	if (full_path == NULL) {
		return (NULL);
	}
	
	strcpy(full_path, dir_path);
	if (dir_path[dir_len - 1] != '/') {
		strcat(full_path, "/");
	}
	strcat(full_path, filename);
	
	return (full_path);
}

static FileInfo *create_file_info(const char *name, const char *full_path, struct stat *st) {
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

static bool add_file_to_array(DynamicArray *arr, const char *entry_name, const char *dir_path) {
	char *full_path = build_path(dir_path, entry_name);
	if (full_path == NULL) {
		return (false);
	}

	struct stat st;
	if (lstat(full_path, &st) != 0) {
		free(full_path);
		return (true);
	}

	FileInfo *file = create_file_info(entry_name, full_path, &st);
	free(full_path);
	
	if (file == NULL) {
		return (false);
	}

	if (!da_push(arr, file)) {
		free_file(file);
		return (false);
	}

	return (true);
}

static DirectoryInfo read_directory(char *path) {
	DirectoryInfo data = {0};
	
	data.path = ft_strdup(path);
	if (data.path == NULL) {
		return ((DirectoryInfo){0});
	}

	DynamicArray *arr = da_create(16);
	if (!arr) {
		free(data.path);
		return ((DirectoryInfo){0});
	}

	DIR *dir = opendir(path);
	if (dir == NULL) {
		da_destroy(arr, free_file);
		free(data.path);
		fprintf(stderr, "opendir failed\n");
		return ((DirectoryInfo){0});
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (should_skip_file(entry->d_name)) {
			continue;
		}

		if (!add_file_to_array(arr, entry->d_name, path)) {
			da_destroy(arr, free_file);
			free(data.path);
			closedir(dir);
			fprintf(stderr, "Failed to add file to directory\n");
			return ((DirectoryInfo){0});
		}
	}

	closedir(dir);
	
	data.files_count = arr->size;
	data.files = (FileInfo **)da_release(arr);
	
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

	if (options & RECURSE) {
		for (size_t i = 0; i < data->files_count; i++) {
			FileInfo *file = data->files[i];			
			if (S_ISDIR(file->stat.st_mode) && !is_file_special(file->name)) {
				char *sub_path = build_path(data->path, file->name);
				if (sub_path == NULL) {
					continue;
				}
				
				printf("\n");
				process_directory(sub_path);
				free(sub_path);
			}
		}
	}
}

static void sort_directory(DirectoryInfo *data) {
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
	free_directory(&data);
}
