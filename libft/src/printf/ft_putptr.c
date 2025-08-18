#include "libft.h"

int ft_putptr_formatted(unsigned long long n, PrintfFormat *fmt) {
	int printed = 0;
	int hex_len = 0;
	unsigned long long temp = n;
	
	if (n == 0) {
		char *nil_str = "(nil)";
		int len = 5;
		int pad = fmt->width - len;
		
		if (!fmt->flags.minus && pad > 0) {
			printed += ft_printf_padding(pad, ' ');
		}
		
		printed += write(1, nil_str, len);
		
		if (fmt->flags.minus && pad > 0) {
			printed += ft_printf_padding(pad, ' ');
		}
	} else {
		temp = n;
		hex_len = (n == 0) ? 1 : 0;
		while (temp) {
			hex_len++;
			temp /= 16;
		}
		
		int total_len = hex_len + 2;
		int pad = fmt->width - total_len;
		
		if (!fmt->flags.minus && pad > 0) {
			printed += ft_printf_padding(pad, fmt->flags.zero ? '0' : ' ');
		}
		
		printed += write(1, "0x", 2);
		printed += ft_puthex(n, 'x');
		
		if (fmt->flags.minus && pad > 0) {
			printed += ft_printf_padding(pad, ' ');
		}
	}
	return (printed);
}