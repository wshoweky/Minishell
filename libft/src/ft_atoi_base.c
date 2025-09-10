/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 12:56:36 by wshoweky          #+#    #+#             */
/*   Updated: 2025/09/03 12:56:43 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	get_base_value(char c, const char *base);

int	ft_atoi_base(const char *nptr, const char *base)
{
	int			i;
	long int	result;
	int			len;

	i = 0;
	result = 0;
	len = ft_strlen(base);
	if (len == 0)
		return (0);
	while (ft_strchr(base, nptr[i]))
	{
		result = result * len + get_base_value(nptr[i], base);
		if (result > INT_MAX)
			return (0);
		i++;
	}
	return (result);
}

static int	get_base_value(char c, const char *base)
{
	int	i;

	i = 0;
	while (base[i] != c)
		i++;
	return (i);
}
