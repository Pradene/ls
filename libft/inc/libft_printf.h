#ifndef PRINTF_H
# define PRINTF_H

typedef struct {
	int minus;	  // '-' flag (left justify)
	int zero;	  // '0' flag (zero padding)
	int hash;	  // '#' flag (alternate form)
	int space;	  // ' ' flag (space for positive numbers)
	int plus;	  // '+' flag (always show sign)
}	PrintfFlags;

typedef enum {
	MOD_NONE,	  // no modifier
	MOD_H,		  // h - short
	MOD_HH,		  // hh - char
	MOD_L,		  // l - long
	MOD_LL,		  // ll - long long
	MOD_J,		  // j - intmax_t
	MOD_Z,		  // z - size_t
	MOD_T,		  // t - ptrdiff_t
	MOD_UPPER_L	  // L - long double
}	PrintfModifier;

typedef enum {
	SPEC_NONE,
	SPEC_C,		  // %c - character
	SPEC_S,		  // %s - string
	SPEC_P,		  // %p - pointer
	SPEC_D,		  // %d - signed decimal
	SPEC_I,		  // %i - signed integer
	SPEC_O,		  // %o - unsigned octal
	SPEC_U,		  // %u - unsigned decimal
	SPEC_X,		  // %x - unsigned hexadecimal (lowercase)
	SPEC_UPPER_X, // %X - unsigned hexadecimal (uppercase)
	SPEC_E,		  // %e - scientific notation (lowercase)
	SPEC_UPPER_E, // %E - scientific notation (uppercase)
	SPEC_F,		  // %f - fixed-point notation
	SPEC_UPPER_F, // %F - fixed-point notation (uppercase)
	SPEC_G,		  // %g - shortest of %e or %f
	SPEC_UPPER_G, // %G - shortest of %E or %F
	SPEC_N,		  // %n - number of characters written so far
	SPEC_PERCENT  // %% - literal percent
}	PrintfSpecifier;

typedef struct {
	PrintfFlags		flags;
	int				width;		// minimum field width
	int				precision;	// precision (number of digits after decimal, max chars for string, etc.)
	PrintfModifier	modifier;
	PrintfSpecifier	specifier;
}	PrintfFormat;

// PRINTF
int	ft_printf(const char *s, ...);
void ft_printf_parse_format(const char *s, int *i, va_list *params, PrintfFormat *fmt);

int	ft_putstr_formatted(char *s, PrintfFormat *fmt);
int	ft_putchar_formatted(char c, PrintfFormat *fmt);
int	ft_puthex_formatted(unsigned long long n, PrintfFormat *fmt, char format);
int	ft_puthex(unsigned long long n, char format);
int	ft_putnbr_formatted(long long n, PrintfFormat *fmt);
int	ft_putunbr_formatted(unsigned int n, PrintfFormat *fmt);
int	ft_putptr_formatted(unsigned long long n, PrintfFormat *fmt);
int ft_putoct_formatted(unsigned long long n, PrintfFormat *fmt);

int	ft_printf_padding(int padding, char c);

#endif