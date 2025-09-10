/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:48:16 by wshoweky          #+#    #+#             */
/*   Updated: 2025/09/10 15:55:50 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *str)
{
	int		i;
	int		sign;
	long	number;

	i = 0;
	number = 0;
	sign = 1;
	while (str[i] && (str[i] == 32 || (str[i] >= 9 && str[i] <= 13)))
		i++;
	if ((str[i] == '-' || str[i] == '+') && str[i++] == '-')
		sign = -1;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (sign == 1 && (number > INT_MAX / 10
				|| (number == INT_MAX / 10 && (str[i] - '0') > 7)))
			return (INT_MAX);
		if (sign == -1 && (number > (long)INT_MAX / 10
				|| (number == (long)INT_MAX / 10 && (str[i] - '0') > 8)))
			return (INT_MIN);
		number = (number * 10) + (str[i++] - '0');
	}
	return ((int)(number * sign));
}
/*
int	ft_atoi(const char *nptr)
{
	int			i;
	int			sign;
	long int	result;

	i = 0;
	sign = 1;
	result = 0;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -sign;
		i++;
		if (!ft_isdigit(nptr[i]))
			return (0);
	}
	while (ft_isdigit(nptr[i]))
	{
		result = result * 10 + (nptr[i] - '0');
		if ((result > INT_MAX && sign == 1) || (-result) < INT_MIN)
			return (0);
		i++;
	}
	result = result * sign;
	return (result);
}
*/
/*
long	ft_atol(const char *nptr)
{
	long	num;
	long	minus;
	int		dig;

	num = 0;
	minus = 1;
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		if (*nptr++ == '-')
			minus = -1;
	while (ft_isdigit(*nptr))
	{
		dig = *nptr - '0';
		if (minus == 1 && (num > LONG_MAX / 10 || (num == LONG_MAX / 10
					&& dig > LONG_MAX % 10)))
			return (LONG_MAX);
		if (minus == -1 && (num > -(LONG_MIN / 10) || (num == -(LONG_MIN / 10)
					&& dig > -(LONG_MIN % 10))))
			return (LONG_MIN);
		num = num * 10 + dig;
		nptr++;
	}
	return (num * minus);
}

int	ft_atoi_safe(const char *nptr, long *result)
{
	long	num;
	long	minus;
	int		dig;

	num = 0;
	minus = 1;
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		if (*nptr++ == '-')
			minus = -1;
	while (ft_isdigit(*nptr))
	{
		dig = *nptr - '0';
		if (minus == 1 && (num > LONG_MAX / 10 || (num == LONG_MAX / 10
					&& dig > LONG_MAX % 10)))
			return (0);
		if (minus == -1 && (num > -(LONG_MIN / 10) || (num == -(LONG_MIN / 10)
					&& dig > -(LONG_MIN % 10))))
			return (0);
		num = num * 10 + dig;
		nptr++;
	}
	*result = num * minus;
	return (1);
}
*/
