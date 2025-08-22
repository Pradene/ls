#include "libft.h"

int ft_printf_padding(int count, char c) {
    int printed = 0;
    while (count-- > 0) {
        ft_putchar(c);
        printed++;
    }
    return (printed);
}

PrintfFormat ft_printf_format_create() {
	PrintfFormat fmt;
    fmt.flags.minus = 0;
    fmt.flags.zero = 0;
    fmt.flags.hash = 0;
    fmt.flags.space = 0;
    fmt.flags.plus = 0;
    fmt.width = 0;
    fmt.precision = -1;
    fmt.modifier = MOD_NONE;
    fmt.specifier = SPEC_NONE;
	return (fmt);
}

long long apply_signed_modifier(va_list *params, PrintfModifier mod) {
    switch (mod) {
        case MOD_HH: return ((signed char)va_arg(*params, int));
        case MOD_H: return ((short)va_arg(*params, int));
        case MOD_L: return (va_arg(*params, long));
        case MOD_LL: return (va_arg(*params, long long));
        case MOD_J: return (va_arg(*params, long long));
        case MOD_Z: return (va_arg(*params, size_t));
        case MOD_T: return (va_arg(*params, ptrdiff_t));
        default: return (va_arg(*params, int));
    }
}

unsigned long long apply_unsigned_modifier(va_list *params, PrintfModifier mod) {
    switch (mod) {
        case MOD_HH: return ((unsigned char)va_arg(*params, unsigned int));
        case MOD_H: return ((unsigned short)va_arg(*params, unsigned int));
        case MOD_L: return (va_arg(*params, unsigned long));
        case MOD_LL: return (va_arg(*params, unsigned long long));
        case MOD_J: return (va_arg(*params, unsigned long long));
        case MOD_Z: return (va_arg(*params, size_t));
        case MOD_T: return (va_arg(*params, ptrdiff_t));
        default: return (va_arg(*params, unsigned int));
    }
}

int ft_printf_format(va_list *params, PrintfFormat *fmt) {
    switch (fmt->specifier) {
        case SPEC_C: return (ft_putchar_formatted(va_arg(*params, int), fmt));
        case SPEC_S: return (ft_putstr_formatted(va_arg(*params, char *), fmt));
        case SPEC_P: return (ft_putptr_formatted(va_arg(*params, unsigned long long), fmt));
        case SPEC_D:
        case SPEC_I: return (ft_putnbr_formatted(apply_signed_modifier(params, fmt->modifier), fmt));
        case SPEC_U: return (ft_putunbr_formatted(apply_unsigned_modifier(params, fmt->modifier), fmt));
        case SPEC_O: return (ft_putoct_formatted(apply_unsigned_modifier(params, fmt->modifier), fmt));
        case SPEC_UPPER_X:
        case SPEC_X: return (ft_puthex_formatted(apply_unsigned_modifier(params, fmt->modifier), fmt, (fmt->specifier == SPEC_X) ? 'x' : 'X'));
        case SPEC_PERCENT: return (ft_putchar_formatted('%', fmt));
        default: return (0);
    }
}

int ft_printf(const char *format, ...) {
    va_list params;
    int printed = 0;
    PrintfFormat fmt;
	
    va_start(params, format);
    
    int i = 0;
    while (format[i]) {
        if (format[i] != '%') {
            printed += ft_putchar_formatted(format[i], NULL);
        } else {
            ++i;
            fmt = ft_printf_format_create();
            ft_printf_parse_format(format, &i, &params, &fmt);
            printed += ft_printf_format(&params, &fmt);
        }
        ++i;
    }
    
    va_end(params);
    return (printed);
}