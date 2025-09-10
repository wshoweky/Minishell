/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:09:45 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/30 12:56:25 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ftlen(long n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count++ ;
	while (n)
	{
		n /= 10;
		count++;
	}
	return (count);
}

static void	ftput(char *str, long n, unsigned int l)
{
	while (n > 0)
	{
		str[--l] = (n % 10) + '0';
		n /= 10;
	}
}

char	*ft_itoa(int n)
{
	char			*s;
	long			ln;
	unsigned int	l;

	ln = n;
	l = ftlen(ln);
	s = (char *)malloc((l + 1) * sizeof(char));
	if (!s)
		return (0);
	if (ln == 0)
		s[0] = '0';
	s[l] = '\0';
	if (ln < 0)
	{
		s[0] = '-';
		ln *= -1;
	}
	ftput(s, ln, l);
	return (s);
}
