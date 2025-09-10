/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:04:07 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/30 12:11:39 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*ts;

	ts = (unsigned char *)s;
	while (n--)
	{
		if (*ts == (unsigned char)c)
			return ((void *)ts);
		ts++;
	}
	return (0);
}
