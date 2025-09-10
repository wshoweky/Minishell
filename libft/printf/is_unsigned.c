/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_unsigned.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 12:29:25 by wshoweky          #+#    #+#             */
/*   Updated: 2025/05/19 13:00:26 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	is_unsigned(unsigned long long n, int base, int cap, int *check)
{
	char	*symbols;
	int		count;

	count = 0;
	if (*check == -1)
		return (-1);
	if (!cap)
		symbols = "0123456789abcdef";
	else
		symbols = "0123456789ABCDEF";
	if (n >= (unsigned long long)base)
		count += is_unsigned(n / base, base, cap, check);
	count += is_char(symbols[n % base], check);
	if (*check == -1)
		return (-1);
	return (count);
}
