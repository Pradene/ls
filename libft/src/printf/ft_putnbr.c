#include "libft.h"

int ft_putnbr_formatted(long long n, PrintfFormat *fmt) {
    int printed = 0;
    int is_negative = (n < 0);
    unsigned long long num = (is_negative) ? -n : n;
    int len = 0;
    unsigned long long temp = num;
    int sign_len = (is_negative || fmt->flags.plus || fmt->flags.space) ? 1 : 0;
    
    if (num == 0) {
        len = 1;
    } else {
        while (temp) {
            len++;
            temp /= 10;
        }
    }
    
    if (fmt->precision == 0 && num == 0) {
        len = 0;
        int total_len = sign_len;
        int pad = fmt->width - total_len;
        
        if (!fmt->flags.minus && pad > 0) {
            printed += ft_printf_padding(pad, ' ');
        }
        
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
        
        if (fmt->flags.minus && pad > 0) {
            printed += ft_printf_padding(pad, ' ');
        }
        
        return printed;
    }
    
    int actual_len = (fmt->precision > len) ? fmt->precision : len;
    int total_len = actual_len + sign_len;
    int pad = fmt->width - total_len;
    
    if (!fmt->flags.minus) {
        if (fmt->flags.zero && fmt->precision < 0) {
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
            if (pad > 0) {
                printed += ft_printf_padding(pad, '0');
            }
        } else {
            if (pad > 0) {
                printed += ft_printf_padding(pad, ' ');
            }
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
    
    if (fmt->precision > len) {
        printed += ft_printf_padding(fmt->precision - len, '0');
    }
    
    if (num == 0) {
        ft_putchar('0');
        printed++;
    } else {
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
    
    if (fmt->flags.minus && pad > 0) {
        printed += ft_printf_padding(pad, ' ');
    }
    
    return (printed);
}