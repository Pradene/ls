#include "libft.h"

int ft_putstr_formatted(char *s, PrintfFormat *fmt) {
    int len, print_len;
    int printed = 0;
    
    if (!s) s = "(null)";
    
    len = ft_strlen(s);
    print_len = (fmt->precision >= 0 && fmt->precision < len) ? fmt->precision : len;
    
    int padding = fmt->width - print_len;
    
    // Right padding (before string)
    if (!fmt->flags.minus && padding > 0) {
        printed += ft_printf_padding(padding, ' ');
    }
    
    // Print string with precision limit
    for (int i = 0; i < print_len; i++) {
        ft_putchar(s[i]);
    }
    printed += print_len;
    
    // Left padding (after string)
    if (fmt->flags.minus && padding > 0) {
        printed += ft_printf_padding(padding, ' ');
    }
    
    return (printed);
}