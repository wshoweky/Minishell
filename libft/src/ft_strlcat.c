/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:20:55 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/30 13:39:25 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *d, const char *s, size_t size)
{
	size_t	dest_len;
	size_t	src_len;

	src_len = ft_strlen(s);
	dest_len = ft_strlen(d);
	if (!d && !size)
		return (src_len);
	if (dest_len >= size)
		return (src_len + size);
	else
		size -= dest_len;
	ft_strlcpy(d + dest_len, s, size);
	return (dest_len + src_len);
}
