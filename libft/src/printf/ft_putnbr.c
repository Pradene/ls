#include "libft.h"

int ft_putnbr_formatted(long long n, PrintfFormat *fmt) {
    int printed = 0;
    int is_negative = (n < 0);
    unsigned long long num = (is_negative) ? -n : n;
    int len = 0;
    unsigned long long temp = num;
    int sign_len = (is_negative || fmt->flags.plus || fmt->flags.space) ? 1 : 0;
    
    // Calculate digit length
    len = (num == 0) ? 1 : 0;
    while (temp) {
        len++;
        temp /= 10;
    }
    
    // Apply precision (minimum digits)
    int actual_len = (fmt->precision > len) ? fmt->precision : len;
    int total_len = actual_len + sign_len;
    int pad = fmt->width - total_len;
    
    // Handle sign and padding
    if (!fmt->flags.minus) {
        if (fmt->flags.zero && fmt->precision < 0) {
            // Sign first, then zero padding
            if (is_negative) {
                ft_putchar('-');
                printed++;
            } else if (fmt->flags.plus) {
                ft_putchar('+');
                printed++;
            } else if (fmt->flags.space) {
                ft_putchar(' ');
                printed++;
            }
            printed += ft_printf_padding(pad, '0');
        } else {
            // Space padding first, then sign
            printed += ft_printf_padding(pad, ' ');
            if (is_negative) {
                ft_putchar('-');
                printed++;
            } else if (fmt->flags.plus) {
                ft_putchar('+');
                printed++;
            } else if (fmt->flags.space) {
                ft_putchar(' ');
                printed++;
            }
        }
    } else {
        // Left align: sign first
        if (is_negative) {
            ft_putchar('-');
            printed++;
        } else if (fmt->flags.plus) {
            ft_putchar('+');
            printed++;
        } else if (fmt->flags.space) {
            ft_putchar(' ');
            printed++;
        }
    }
    
    // Print precision zeros
    if (fmt->precision > len) {
        printed += ft_printf_padding(fmt->precision - len, '0');
    }
    
    // Print digits
    if (num == 0) {
        ft_putchar('0');
        printed++;
    } else {
        // Convert to string and print
        char digits[21];
        int i = 20;
        digits[i] = '\0';
        while (num) {
            digits[--i] = '0' + (num % 10);
            num /= 10;
        }
        ft_putstr(&digits[i]);
        printed += ft_strlen(&digits[i]);
    }
    
    // Left alignment padding
    if (fmt->flags.minus && pad > 0) {
        printed += ft_printf_padding(pad, ' ');
    }
    
    return (printed);
}