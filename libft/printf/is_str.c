/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_str.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 12:25:24 by wshoweky          #+#    #+#             */
/*   Updated: 2025/05/16 18:49:26 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	is_str(char *str, int *check)
{
	int	count;

	count = 0;
	if (*check == -1)
		return (-1);
	if (!str)
		str = "(null)";
	while (*str)
	{
		count += is_char(*str, check);
		if (*check == -1)
			return (-1);
		str++;
	}
	return (count);
}
