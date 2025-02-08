#include "ls.h"

int compare_file_name(const void *a, const void *b) {
    const t_file *file_a = *(const t_file **)a;
    const t_file *file_b = *(const t_file **)b;
    const char *name_a = file_a->name;
    const char *name_b = file_b->name;

    // For other entries, skip ALL leading dots before comparison
    const char *str1 = name_a;
    const char *str2 = name_b;

    while (*str1 == '.') str1++;
    while (*str2 == '.') str2++;

    // Handle empty strings after skipping dots
    if (*str1 == '\0' && *str2 == '\0') return 0;
    if (*str1 == '\0') return -1;
    if (*str2 == '\0') return 1;

    // Case-insensitive comparison
    char *l1 = ft_strtolower(str1);
    char *l2 = ft_strtolower(str2);
    int result = ft_strcmp(l1, l2);
    
    free(l1);
    free(l2);

    return result;
}

int compare_file_mtime(const void *a, const void *b) {
    return (*(t_file **)b)->stat.st_mtime - (*(t_file **)a)->stat.st_mtime;
}

int compare_name(const void *a, const void *b) {
    const char *name_a = *(const char **)a;
    const char *name_b = *(const char **)b;

    // For other entries, skip ALL leading dots before comparison
    const char *str1 = name_a;
    const char *str2 = name_b;

    while (*str1 == '.') str1++;
    while (*str2 == '.') str2++;

    // Handle empty strings after skipping dots
    if (*str1 == '\0' && *str2 == '\0') return 0;
    if (*str1 == '\0') return -1;
    if (*str2 == '\0') return 1;

    // Case-insensitive comparison
    char *l1 = ft_strtolower(str1);
    char *l2 = ft_strtolower(str2);
    int result = ft_strcmp(l1, l2);
    
    free(l1);
    free(l2);

    return result;
}