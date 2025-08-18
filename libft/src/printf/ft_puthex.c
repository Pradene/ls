#include "libft.h"

static int put_hex_digits(unsigned long long n, char c) {
	char digits[] = "0123456789abcdef";
	if (c == 'X') {
		digits[10] = 'A';
		digits[11] = 'B';
		digits[12] = 'C';
		digits[13] = 'D';
		digits[14] = 'E';
		digits[15] = 'F';
	}
	
	if (n >= 16) {
		return (put_hex_digits(n / 16, c) + put_hex_digits(n % 16, c));
	} else {
		ft_putchar(digits[n]);
		return (1);
	}
}

int ft_puthex(unsigned long long n, char c) {
	if (n == 0) {
		ft_putchar('0');
		return (1);
	} else {
		return (put_hex_digits(n, c));
	}
}

int ft_puthex_formatted(unsigned long long n, PrintfFormat *fmt, char c) {
	int printed = 0;
	int len = 0;
	unsigned long long temp = n;
	int prefix_len = (fmt->flags.hash && n != 0) ? 2 : 0;
	
	if (n == 0) {
		len = 1;
	} else {
		while (temp) {
			len++;
			temp /= 16;
		}
	}
	
	if (fmt->precision == 0 && n == 0) {
		len = 0;
	}
	
	int actual_len = (fmt->precision > len) ? fmt->precision : len;
	int total_len = actual_len + prefix_len;
	int pad = fmt->width - total_len;
	
	if (!fmt->flags.minus && pad > 0) {
		char pad_char = (fmt->flags.zero && fmt->precision < 0) ? '0' : ' ';
		if (pad_char == '0' && prefix_len > 0) {
			ft_putchar('0');
			ft_putchar(c == 'x' ? 'x' : 'X');
			printed += 2;
			prefix_len = 0;
		}
		printed += ft_printf_padding(pad, pad_char);
	}
	
	if (prefix_len > 0) {
		ft_putchar('0');
		ft_putchar(c == 'x' ? 'x' : 'X');
		printed += 2;
	}
	
	if (fmt->precision > len) {
		printed += ft_printf_padding(fmt->precision - len, '0');
	}
	
	if (!(fmt->precision == 0 && n == 0)) {
		printed += ft_puthex(n, c);
	}
	
	if (fmt->flags.minus && pad > 0) {
		printed += ft_printf_padding(pad, ' ');
	}
	
	return (printed);
}