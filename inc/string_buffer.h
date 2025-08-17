#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	char    *buffer;
	size_t  size;
	size_t  pos;
}   StringBuffer;

StringBuffer    *create_string_buffer(size_t initial_size);
int             append_to_buffer(StringBuffer *sb, const char *format, ...);
void            free_string_buffer(StringBuffer *sb);

#endif