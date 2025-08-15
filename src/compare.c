#include "ls.h"

int compare_file_name(const void *a, const void *b) {
    const FileInfo *file_a = *(const FileInfo **)a;
    const FileInfo *file_b = *(const FileInfo **)b;
    char *s1 = file_a->name;
    char *s2 = file_b->name;

    char *t1 = ft_strdup(s1);
    char *t2 = ft_strdup(s2);

    ft_strtolower(t1);
    ft_strtolower(t2);

    int result = ft_strcmp(t1, t2);

    free(t1);
    free(t2);
    
    return (result);
}

int compare_file_mtime(const void *a, const void *b) {
    return (*(FileInfo **)b)->stat.st_mtime - (*(FileInfo **)a)->stat.st_mtime;
}

int compare_file_size(const void *a, const void *b) {
    return (*(FileInfo **)b)->stat.st_size - (*(FileInfo **)a)->stat.st_size;
}

int compare_name(const void *a, const void *b) {
    char *t1 = *(char **)a;
    char *t2 = *(char **)b;

    char *s1 = ft_strdup(t1);
    char *s2 = ft_strdup(t2);

    ft_strtolower(s1);
    ft_strtolower(s2);

    int result = ft_strcmp(s1, s2);

    free(s1);
    free(s2);

    return (result);
}
