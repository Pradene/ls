#include "libft.h"

static bool is_flag(char c) {
	return (c == '-' || c == '0' || c == '#' || c == ' ' || c == '+');
}

static void ft_printf_parse_flags(const char *s, int *i, PrintfFormat *fmt) {
    while (s[*i] && is_flag(s[*i])) {
        if (s[*i] == '-') fmt->flags.minus = 1;
        else if (s[*i] == '0') fmt->flags.zero = 1;
        else if (s[*i] == '#') fmt->flags.hash = 1;
        else if (s[*i] == ' ') fmt->flags.space = 1;
        else if (s[*i] == '+') fmt->flags.plus = 1;
        (*i)++;
    }
    
    // Handle flag conflicts
    if (fmt->flags.minus) fmt->flags.zero = 0;
    if (fmt->flags.plus) fmt->flags.space = 0;
}

static void ft_printf_parse_width(const char *s, int *i, va_list *params, PrintfFormat *fmt) {
    if (s[*i] == '*') {
        fmt->width = va_arg(*params, int);
        if (fmt->width < 0) {
            fmt->flags.minus = 1;
            fmt->width = -fmt->width;
        }
        (*i)++;
    } else {
        while (s[*i] >= '0' && s[*i] <= '9') {
            fmt->width = fmt->width * 10 + (s[*i] - '0');
            (*i)++;
        }
    }
}

static void ft_printf_parse_precision(const char *s, int *i, va_list *params, PrintfFormat *fmt) {
    if (s[*i] == '.') {
        (*i)++;
        fmt->precision = 0;
        if (s[*i] == '*') {
            fmt->precision = va_arg(*params, int);
            if (fmt->precision < 0) fmt->precision = -1;
            (*i)++;
        } else {
            while (s[*i] >= '0' && s[*i] <= '9') {
                fmt->precision = fmt->precision * 10 + (s[*i] - '0');
                (*i)++;
            }
        }
    }
}

static void ft_printf_parse_modifier(const char *s, int *i, PrintfFormat *fmt) {
    if (s[*i] == 'h') {
        if (s[*i + 1] == 'h') {
            fmt->modifier = MOD_HH;
            (*i) += 2;
        } else {
            fmt->modifier = MOD_H;
            (*i)++;
        }
    } else if (s[*i] == 'l') {
        if (s[*i + 1] == 'l') {
            fmt->modifier = MOD_LL;
            (*i) += 2;
        } else {
            fmt->modifier = MOD_L;
            (*i)++;
        }
    } else if (s[*i] == 'j') {
        fmt->modifier = MOD_J;
        (*i)++;
    } else if (s[*i] == 'z') {
        fmt->modifier = MOD_Z;
        (*i)++;
    } else if (s[*i] == 't') {
        fmt->modifier = MOD_T;
        (*i)++;
    } else if (s[*i] == 'L') {
        fmt->modifier = MOD_UPPER_L;
        (*i)++;
    }
}

static void ft_printf_parse_specifier(const char *s, int *i, PrintfFormat *fmt) {
    char c = s[*i];
    
    switch (c) {
        case 'c': fmt->specifier = SPEC_C; break;
        case 's': fmt->specifier = SPEC_S; break;
        case 'p': fmt->specifier = SPEC_P; break;
        case 'd': fmt->specifier = SPEC_D; break;
        case 'i': fmt->specifier = SPEC_I; break;
        case 'o': fmt->specifier = SPEC_O; break;
        case 'u': fmt->specifier = SPEC_U; break;
        case 'x': fmt->specifier = SPEC_X; break;
        case 'X': fmt->specifier = SPEC_UPPER_X; break;
        case 'e': fmt->specifier = SPEC_E; break;
        case 'E': fmt->specifier = SPEC_UPPER_E; break;
        case 'f': fmt->specifier = SPEC_F; break;
        case 'F': fmt->specifier = SPEC_UPPER_F; break;
        case 'g': fmt->specifier = SPEC_G; break;
        case 'G': fmt->specifier = SPEC_UPPER_G; break;
        case 'n': fmt->specifier = SPEC_N; break;
        case '%': fmt->specifier = SPEC_PERCENT; break;
        default: fmt->specifier = SPEC_NONE; break;
    }
}

void ft_printf_parse_format(const char *s, int *i, va_list *params, PrintfFormat *fmt) {
    ft_printf_parse_flags(s, i, fmt);
    ft_printf_parse_width(s, i, params, fmt);
    ft_printf_parse_precision(s, i, params, fmt);
    ft_printf_parse_modifier(s, i, fmt);
    ft_printf_parse_specifier(s, i, fmt);
}
