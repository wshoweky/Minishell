/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena_itoa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 23:10:11 by gita              #+#    #+#             */
/*   Updated: 2025/10/06 23:10:17 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ftlen(long n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count++;
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

char	*ar_itoa(t_arena *arena, int n)
{
	char			*s;
	long			ln;
	unsigned int	l;

	ln = n;
	l = ftlen(ln);
	s = ar_alloc(arena, (l + 1) * sizeof(char));
	if (!s)
		return (NULL);
	if (ln == 0)
		s[0] = '0';
	s[l] = '\0';
	if (ln < 0)
	{
		s[0] = '-';
		if (!s)
			return (NULL);
		ln *= -1;
	}
	ftput(s, ln, l);
	return (s);
}
