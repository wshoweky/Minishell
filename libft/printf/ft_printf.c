/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 13:38:04 by wshoweky          #+#    #+#             */
/*   Updated: 2025/05/19 13:02:12 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	check_format(const char *format, va_list ap, int *check)
{
	int	count;

	count = 0;
	if (*format == '%')
		count += is_char('%', check);
	else if (*format == 'c')
		count += is_char(va_arg(ap, int), check);
	else if (*format == 's')
		count += is_str(va_arg(ap, char *), check);
	else if (*format == 'i' || *format == 'd')
		count += is_signed(va_arg(ap, int), check);
	else if (*format == 'p')
		count += is_ptr(va_arg(ap, void *), check);
	else if (*format == 'u')
		count += is_unsigned(va_arg(ap, unsigned int), 10, 0, check);
	else if (*format == 'x')
		count += is_unsigned(va_arg(ap, unsigned int), 16, 0, check);
	else if (*format == 'X')
		count += is_unsigned(va_arg(ap, unsigned int), 16, 1, check);
	else
		return (-1);
	return (count);
}

int	ft_printf(const char *format, ...)
{
	va_list		ap;
	int			count;
	int			check;

	check = 0;
	count = 0;
	if (!format)
		return (-1);
	va_start (ap, format);
	while (*format)
	{
		if (*format == '%')
			count += check_format(++format, ap, &check);
		else
			count += is_char(*format, &check);
		if (!*format)
			return (va_end(ap), 0);
		format++;
	}
	va_end(ap);
	if (check == -1)
		return (-1);
	return (count);
}
