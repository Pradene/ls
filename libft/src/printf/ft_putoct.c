#include "libft.h"

int ft_putoct_formatted(unsigned long long n, PrintfFormat *fmt) {
    int printed = 0;
    int len = 0;
    unsigned long long temp = n;
    
    // Calculate length
    len = (n == 0) ? 1 : 0;
    while (temp) {
        len++;
        temp /= 8;
    }
    
    int prefix_len = (fmt->flags.hash && n != 0) ? 1 : 0;
    int actual_len = (fmt->precision > len) ? fmt->precision : len;
    int total_len = actual_len + prefix_len;
    int pad = fmt->width - total_len;
    
    // Right padding
    if (!fmt->flags.minus && !fmt->flags.zero && pad > 0) {
        printed += ft_printf_padding(pad, ' ');
    }
    
    // Prefix
    if (prefix_len) {
        ft_putchar('0');
        printed++;
    }
    
    // Zero padding
    if (!fmt->flags.minus && fmt->flags.zero && pad > 0) {
        printed += ft_printf_padding(pad, '0');
    }
    
    // Precision zeros
    if (fmt->precision > len) {
        printed += ft_printf_padding(fmt->precision - len, '0');
    }
    
    // Print digits
    if (n == 0) {
        ft_putchar('0');
        printed++;
    } else {
        char digits[23];
        int i = 22;
        digits[i] = '\0';
        while (n) {
            digits[--i] = '0' + (n % 8);
            n /= 8;
        }
        ft_putstr(&digits[i]);
        printed += ft_strlen(&digits[i]);
    }
    
    // Left padding
    if (fmt->flags.minus && pad > 0) {
        printed += ft_printf_padding(pad, ' ');
    }
    
    return printed;
}