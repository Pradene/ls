#include "string_buffer.h"

StringBuffer *create_string_buffer(size_t initial_size) {
	StringBuffer *sb = malloc(sizeof(StringBuffer));
	if (!sb) {
		return (NULL);
	}
	
	sb->buffer = malloc(initial_size);
	if (!sb->buffer) {
		free(sb);
		return (NULL);
	}
	
	sb->size = initial_size;
	sb->pos = 0;
	return (sb);
}

int append_to_buffer(StringBuffer *sb, const char *format, ...) {
	va_list args;
	va_start(args, format);
	
	int needed = vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	if (sb->pos + needed + 1 > sb->size) {
		size_t new_size = sb->size * 2;
		while (new_size < sb->pos + needed + 1) {
			new_size *= 2;
		}
		
		char *new_buffer = realloc(sb->buffer, new_size);
		if (!new_buffer) {
			return (-1);
		}
		
		sb->buffer = new_buffer;
		sb->size = new_size;
	}
	
	va_start(args, format);
	int written = vsnprintf(sb->buffer + sb->pos, sb->size - sb->pos, format, args);
	va_end(args);
	
	sb->pos += written;
	return (written);
}

void free_string_buffer(StringBuffer *sb) {
	if (!sb) {
		return;
	}
	free(sb->buffer);
	free(sb);
}