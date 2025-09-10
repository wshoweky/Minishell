/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_ptr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 12:47:46 by wshoweky          #+#    #+#             */
/*   Updated: 2025/05/19 11:30:47 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	is_ptr(void *ptr, int *check)
{
	int	count;

	count = 0;
	if (!ptr)
	{
		count += is_str("(nil)", check);
		return (count);
	}
	if (*check == -1)
		return (-1);
	count += is_str("0x", check);
	if (count == -1)
		return (-1);
	count += is_unsigned((unsigned long long)ptr, 16, 0, check);
	if (*check == -1)
		return (-1);
	return (count);
}
