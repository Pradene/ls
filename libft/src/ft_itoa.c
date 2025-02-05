/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpradene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 18:03:32 by lpradene          #+#    #+#             */
/*   Updated: 2022/11/09 12:52:29 by lpradene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	absolute(int n)
{
	if (n < 0)
		return (-n);
	return (n);
}

static int	count(int n)
{
	int	c;
	int	d;

	d = 1;
	c = 0;
	if (n < 0)
		c++;
	while (n || d == 1)
	{
		d = 0;
		n /= 10;
		c++;
	}
	return (c);
}

char	*ft_itoa(int n)
{
	int		c;
	char	*str;
	int		d;

	d = 1;
	c = count(n);
	str = ft_calloc(c + 1, sizeof(char));
	if (!str)
		return (0);
	if (n < 0)
		*str = '-';
	while (n || d == 1)
	{
		d = 0;
		c--;
		str[c] = absolute(n % 10) + 48;
		n /= 10;
	}
	return (str);
}
