#include "ls.h"

extern Options options;
extern SortType sort_type;
extern ShowType show_type;

static void free_file(FileInfo *file) {
	if (!file) return;
	free(file->link);
	free(file->name);
}

static void free_directory(DirectoryInfo *directory) {
	if (!directory) return;
	ft_da_foreach(&directory->files, file, FileInfo) {
		free_file(file);
	}
	
	ft_da_free(directory->files);
	free(directory->path);
}

static bool is_file_hidden(const char *name) {
	return (name[0] == '.');
}

static bool is_file_special(const char *name) {
	return (ft_strcmp(name, ".") == 0 || ft_strcmp(name, "..") == 0);
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
	if (!full_path) {
		return (NULL);
	}
	
	ft_strcpy(full_path, dir_path);
	if (dir_path[dir_len - 1] != '/') {
		ft_strcat(full_path, "/");
	}
	ft_strcat(full_path, filename);
	
	return (full_path);
}

static FileInfo create_file_info(const char *name, const char *full_path, struct stat *st) {
	FileInfo file = {0};
	file.name = ft_strdup(name);
	if (!file.name) {
		return ((FileInfo){0});
	}
	
	file.link = NULL;
	file.stat = *st;
	
	if (S_ISLNK(st->st_mode)) {
		char *link_target = malloc(1024);
		if (link_target) {
			ssize_t len = readlink(full_path, link_target, 1023);
			if (len > 0) {
				link_target[len] = '\0';
				file.link = link_target;
			} else {
				free(link_target);
			}
		}
	}
	
	return (file);
}

static bool directory_add_file(DirectoryInfo *directory, const char *filename) {
	char *path = build_path(directory->path, filename);
	if (!path) {
		return (false);
	}

	struct stat st;
	if (lstat(path, &st)) {
		free(path);
		return (true);
	}

	FileInfo file = create_file_info(filename, path, &st);
	
	free(path);

	if (!ft_da_append(&directory->files, file)) {
		free_file(&file);
		return (false);
	}

	return (true);
}

static bool read_directory(char *path, DirectoryInfo *directory) {	
	directory->path = ft_strdup(path);
	if (!directory->path) {
		return (false);
	}

	DIR *dir = opendir(path);
	if (!dir) {
		fprintf(stderr, "ft_ls: cannot open directory '%s': %s\n", path, strerror(errno));
		free_directory(directory);
		return (false);
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (should_skip_file(entry->d_name) == true) {
			continue;
		}

		if (directory_add_file(directory, entry->d_name) == false) {
			fprintf(stderr, "ft_ls: failed to add file to directory\n");
			free_directory(directory);
			closedir(dir);
			return (false);
		}
	}

	closedir(dir);
	return (true);
}

static void print_directory(DirectoryInfo *directory) {
	if (options & RECURSE) {
		ft_printf("%s:\n", directory->path);
	}
	
	if ((options & LIST) || (options & LIST_GROUP_ONLY)) {
		print_list_formatted(directory);
	} else {
		print_formatted(directory);
	}

	if (options & RECURSE) {
		ft_da_foreach(&directory->files, file, FileInfo) {
			if (S_ISDIR(file->stat.st_mode) && !is_file_special(file->name)) {
				char *sub_path = build_path(directory->path, file->name);
				if (!sub_path) continue;
				ft_printf("\n");
				process_directory(sub_path);
				free(sub_path);
			}
		}
	}
}

static void sort_directory(DirectoryInfo *directory) {
	void	*data = directory->files.items;
	size_t	size = ft_da_size(&directory->files);

	switch (sort_type) {
		case SORT_NONE: break;
		case SORT_MTIME: ft_quicksort(data, size, sizeof(FileInfo), compare_file_mtime); break;
		case SORT_ATIME: ft_quicksort(data, size, sizeof(FileInfo), compare_file_atime); break;
		case SORT_SIZE: ft_quicksort(data, size, sizeof(FileInfo), compare_file_size); break;
		case SORT_NAME:
		default: ft_quicksort(data, size, sizeof(FileInfo), compare_file_name); break;
	}

	if (options & REVERSE) {
		ft_reverse(data, size, sizeof(FileInfo));
	}
}

void process_directory(char *path) {
	DirectoryInfo directory = {0};
	if (read_directory(path, &directory) == false) return;
	sort_directory(&directory);
	print_directory(&directory);
	free_directory(&directory);
}
