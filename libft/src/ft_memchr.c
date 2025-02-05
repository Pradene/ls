/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpradene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 16:41:41 by lpradene          #+#    #+#             */
/*   Updated: 2022/11/08 11:38:35 by lpradene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *mem, int c, size_t n)
{
	size_t	i;

	if (!mem)
		return (0);
	i = 0;
	while (i < n)
	{
		if (((char *)mem)[i] == (char)c)
			return ((char *)mem + i);
		i++;
	}
	return (0);
}
