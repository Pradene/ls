/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpradene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 14:55:53 by lpradene          #+#    #+#             */
/*   Updated: 2022/11/08 11:52:08 by lpradene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n - 1 && n != 0)
	{
		if (src[i] != 0)
		{
			dest[i] = src[i];
			i++;
		}
		else
			break ;
	}
	if (n)
	{
		dest[i] = 0;
		i++;
	}
	return (ft_strlen(src));
}
