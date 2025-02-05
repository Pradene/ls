/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpradene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 16:41:55 by lpradene          #+#    #+#             */
/*   Updated: 2022/11/08 11:39:50 by lpradene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	absolute(int n)
{
	if (n < 0)
		return (-n);
	return (n);
}

int	ft_memcmp(const void *mem1, const void *mem2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		if (((char *)mem1)[i] != ((char *)mem2)[i])
			return (absolute(((char *)mem1)[i]) - absolute(((char *)mem2)[i]));
		i++;
	}
	return (0);
}
