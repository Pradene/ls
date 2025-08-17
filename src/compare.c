#include "ls.h"

int compare_file_name(const void *a, const void *b) {
	if (!a || !b) return (0);

	const FileInfo *file_a = *(const FileInfo **)a;
	const FileInfo *file_b = *(const FileInfo **)b;

	if (!file_a || !file_b) return (0);
	if (!file_a->name || !file_b->name) return (0);

	char *t1 = ft_strdup(file_a->name);
	char *t2 = ft_strdup(file_b->name);

	if (!t1 || !t2) {
		if (t1) free(t1);
		if (t2) free(t2);
		return (0);
	}

	ft_strtolower(t1);
	ft_strtolower(t2);

	int result = ft_strcmp(t1, t2);

	free(t1);
	free(t2);

	return (result);
}

int compare_file_mtime(const void *a, const void *b) {
	if (!a || !b) return (0);

	const FileInfo *file_a = *(const FileInfo **)a;
	const FileInfo *file_b = *(const FileInfo **)b;
	
	if (!file_a || !file_b) return (0);
		
	time_t time_a = file_a->stat.st_mtime;
	time_t time_b = file_b->stat.st_mtime;
	
	if (time_a < time_b) return (1);
	if (time_a > time_b) return (-1);
	return (0);
}

int compare_file_size(const void *a, const void *b) {
	if (!a || !b) return (0);

	const FileInfo *file_a = *(const FileInfo **)a;
	const FileInfo *file_b = *(const FileInfo **)b;
	
	if (!file_a || !file_b) return (0);
	
	off_t size_a = file_a->stat.st_size;
	off_t size_b = file_b->stat.st_size;
		
	if (size_a < size_b) return (1);
	if (size_a > size_b) return (-1);
	return (0);
}

int compare_name(const void *a, const void *b) {
	if (!a || !b) return (0);

	char *t1 = *(char **)a;
	char *t2 = *(char **)b;
		
	if (!t1 || !t2) return (0);

	char *s1 = ft_strdup(t1);
	char *s2 = ft_strdup(t2);
	
	if (!s1 || !s2) {
		if (s1) free(s1);
		if (s2) free(s2);
		return (0);
	}

	ft_strtolower(s1);
	ft_strtolower(s2);

	int result = ft_strcmp(s1, s2);

	free(s1);
	free(s2);

	return (result);
}