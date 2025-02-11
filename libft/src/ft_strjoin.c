/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpradene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:48:26 by lpradene          #+#    #+#             */
/*   Updated: 2022/11/08 11:48:46 by lpradene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char *s1, char const *s2)
{
	int		i;
	int		size;
	char	*str;

	i = 0;
	size = ft_strlen(s1);
	str = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!str)
		return (0);
	str = ft_memcpy(str, s1, ft_strlen(s1));
	while (i < ft_strlen(s2))
	{
		str[i + size] = s2[i];
		i++;
	}

	str[i + size] = 0;
	return (str);
}
