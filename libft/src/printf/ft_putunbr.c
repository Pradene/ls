#include "libft.h"

static int put_unsigned_digits(unsigned long long n) {
	int printed = 0;
	if (n >= 10) {
		printed += put_unsigned_digits(n / 10);
		ft_putchar('0' + n % 10);
	} else {
		ft_putchar('0' + n);
	}
	return (printed + 1);
}

int ft_putunbr_formatted(unsigned long long n, PrintfFormat *fmt) {
	int printed = 0;
	int len = 0;
	unsigned long long temp = n;
	
	if (n == 0) {
		len = 1;
	} else {
		while (temp) {
			len++;
			temp /= 10;
		}
	}
	
	if (fmt->precision == 0 && n == 0) {
		len = 0;
		int pad = fmt->width;
		
		if (!fmt->flags.minus && pad > 0) {
			printed += ft_printf_padding(pad, ' ');
		}
		
		if (fmt->flags.minus && pad > 0) {
			printed += ft_printf_padding(pad, ' ');
		}
		
		return printed;
	}
	
	int actual_len = (fmt->precision > len) ? fmt->precision : len;
	int pad = fmt->width - actual_len;
	
	if (!fmt->flags.minus && pad > 0) {
		char pad_char = (fmt->flags.zero && fmt->precision < 0) ? '0' : ' ';
		printed += ft_printf_padding(pad, pad_char);
	}
	
	if (fmt->precision > len) {
		printed += ft_printf_padding(fmt->precision - len, '0');
	}
	
	if (n == 0) {
		ft_putchar('0');
		printed += 1;
	} else {
		printed += put_unsigned_digits(n);
	}
	
	if (fmt->flags.minus && pad > 0) {
		printed += ft_printf_padding(pad, ' ');
	}
	
	return printed;
}