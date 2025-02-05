/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpradene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 14:56:07 by lpradene          #+#    #+#             */
/*   Updated: 2022/11/08 11:50:29 by lpradene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t n)
{
	int		ldest;
	size_t	i;
	size_t	j;

	i = 0;
	while (n - i != 0 && dest[i] != 0)
		i++;
	ldest = i;
	if (n - i == 0)
		return (ldest + ft_strlen(src));
	j = 0;
	while (src[j] != 0)
	{
		if (n - i != 1)
		{
			dest[i] = src[j];
			i++;
		}
		j++;
	}
	dest[i] = 0;
	return (ldest + ft_strlen(src));
}
