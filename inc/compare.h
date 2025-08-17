#ifndef COMPARE_H
#define COMPARE_H

int compare_name(const void *a, const void *b);
int compare_file_name(const void *a, const void *b);
int compare_file_size(const void *a, const void *b);
int compare_file_mtime(const void *a, const void *b);
int compare_file_atime(const void *a, const void *b);

#endif