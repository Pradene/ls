#include "libft.h"

void format_init(PrintFormat *fmt) {
	fmt->width = 0;
	fmt->space = 0;
	fmt->minus = 0;
	fmt->zero = 0;
	fmt->hash = 0;
	fmt->plus = 0;
}

static bool format_is_flag(char c) {
	return (c == '-' || c == '0' || c == '#' || c == ' ' || c == '+');
}

void format_parse_flags(const char *s, int *i, PrintFormat *fmt, va_list *params) {
	while (s[*i] && format_is_flag(s[*i])) {
		if (s[*i] == '-') { fmt->minus = 1; } 
		else if (s[*i] == '0') { fmt->zero = 1; }
		else if (s[*i] == '#') { fmt->hash = 1; }
		else if (s[*i] == ' ') { fmt->space = 1; }
		else if (s[*i] == '+') { fmt->plus = 1; }
		(*i)++;
	}

	if (s[*i] == '*') {
		fmt->width = va_arg(*params, int);
		if (fmt->width < 0) {
			fmt->minus = 1;
			fmt->width = -fmt->width;
		}
		(*i)++;
	} else {
		while (s[*i] >= '0' && s[*i] <= '9') {
			fmt->width = fmt->width * 10 + (s[*i] - '0');
			(*i)++;
		}
	}
	
	if (fmt->minus != 0) fmt->zero = 0;
	if (fmt->plus != 0)  fmt->space = 0;
}

int format_print_padding(int count, char c) {
	int printed = 0;
	while (count-- > 0) {
		ft_putchar(c);
		printed++;
	}
	return (printed);
}