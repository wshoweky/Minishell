/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:09:57 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/30 13:35:23 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	size_t	bytes;
	void	*tmp;

	if (!count || !size)
		return (malloc(0));
	bytes = count * size;
	if (count != 0 && bytes / count != size)
		return (0);
	tmp = malloc(bytes);
	if (!tmp)
		return (0);
	ft_bzero(tmp, bytes);
	return (tmp);
}
