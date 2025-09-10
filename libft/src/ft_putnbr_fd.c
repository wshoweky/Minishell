/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:09:47 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/30 13:04:20 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
static void	ftputc(char c, int fd)
{
	write (fd, &c, 1);
}*/

void	ft_putnbr_fd(int n, int fd)
{
	long	l;

	l = n;
	if (l < 0)
	{
		ft_putchar_fd ('-', fd);
		l = -l;
	}
	if (l > 9)
		ft_putnbr_fd(l / 10, fd);
	l %= 10;
	ft_putchar_fd (l + 48, fd);
}
