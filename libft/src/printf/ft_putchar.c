#include "libft.h"

int ft_putchar_formatted(char c, PrintfFormat *fmt) {
	int printed = 0;
	int padding = (fmt != NULL) ? (fmt->width - 1) : 0;
	
	if (padding > 0) {
		if (fmt->flags.minus) {
			ft_putchar(c);
			printed += ft_printf_padding(padding, ' ');
		} else {
			printed += ft_printf_padding(padding, ' ');
			ft_putchar(c);
		}
	} else {
		ft_putchar(c);
	}
	return (printed + 1);
}