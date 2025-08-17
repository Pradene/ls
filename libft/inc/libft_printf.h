#ifndef PRINTF_H
# define PRINTF_H

typedef struct {
	int minus;	  // '-' flag (left justify)
	int zero;	  // '0' flag (zero padding)
	int hash;	  // '#' flag (alternate form)
	int space;	  // ' ' flag (space for positive numbers)
	int plus;	  // '+' flag (always show sign)
	int width;	  // minimum field width
} PrintFormat;

// PRINTF
int	ft_printf(const char *s, ...);
int	ft_format(va_list params, PrintFormat *fmt, char c);
int	ft_putstr_formatted(char *s, PrintFormat *fmt);
int	ft_putchar_formatted(char c, PrintFormat *fmt);
int	ft_puthex_formatted(unsigned long long n, PrintFormat *fmt, char format);
int	ft_puthex(unsigned long long n, char format);
int	ft_putnbr_formatted(int n, PrintFormat *fmt);
int	ft_putunbr_formatted(unsigned int n, PrintFormat *fmt);
int	ft_putptr_formatted(unsigned long long n, PrintFormat *fmt);
int	ft_putsize_t_formatted(size_t n, PrintFormat *fmt);

void format_init(PrintFormat *fmt);
void format_parse_flags(const char *s, int *i, PrintFormat *fmt, va_list *params);
int	format_print_padding(int padding, char c);

#endif