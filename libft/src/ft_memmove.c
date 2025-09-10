/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:13:42 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/28 19:42:40 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	char	*c_src;
	char	*c_dst;
	size_t	i;

	if (dst == src)
		return (dst);
	if (!dst && !src)
		return (NULL);
	c_src = (char *) src;
	c_dst = (char *) dst;
	i = -1;
	if (c_dst > c_src)
	{
		while (len > 0)
		{
			len--;
			c_dst[len] = c_src[len];
		}
	}
	else
	{
		while (++i < len)
			c_dst[i] = c_src[i];
	}
	return (dst);
}
