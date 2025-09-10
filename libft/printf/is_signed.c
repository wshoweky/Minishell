/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_signed.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 12:35:47 by wshoweky          #+#    #+#             */
/*   Updated: 2025/05/16 18:49:36 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	is_signed(int n, int *check)
{
	long	l;
	char	c;
	int		count;

	count = 0;
	l = n;
	if (l < 0)
	{
		count += is_char('-', check);
		l = -l;
	}
	if (l >= 10)
		count += is_signed(l / 10, check);
	c = (l % 10) + '0';
	count += is_char(c, check);
	if (*check == -1)
		return (-1);
	return (count);
}
