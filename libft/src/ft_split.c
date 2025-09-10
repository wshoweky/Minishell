/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:55:45 by wshoweky          #+#    #+#             */
/*   Updated: 2025/07/28 14:33:03 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_free(char **str)
{
	size_t	i;

	i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
}

static int	word_count(char const *s, char c)
{
	int	count;

	count = 0;
	while (*s)
	{
		if (*s == c)
			s++;
		else
		{
			count++;
			while (*s && *s != c)
				s++;
		}
	}
	return (count);
}

static char	*fill_word(char *t, char const *s, int index, int len)
{
	int	i;

	i = 0;
	while (len > 0)
	{
		t[i] = s[index - len];
		i++;
		len--;
	}
	t[i] = '\0';
	return (t);
}

static char	**ft_split_words(char const *s, char c, char **str, int words_count)
{
	int	i;
	int	word;
	int	word_len;

	i = 0;
	word = 0;
	word_len = 0;
	while (word < words_count)
	{
		while (s[i] && s[i] == c)
			i++;
		while (s[i] && s[i] != c)
		{
			i++;
			word_len++;
		}
		str[word] = (char *)malloc(sizeof(char) * (word_len + 1));
		if (!str[word])
			ft_free(str);
		fill_word(str[word], s, i, word_len);
		word_len = 0;
		word++;
	}
	str[word] = 0;
	return (str);
}

char	**ft_split(char const *s, char c)
{
	char	**str;
	int		count;

	count = word_count(s, c);
	str = (char **)malloc((count + 1) * sizeof(char *));
	if (!str || !s)
		return (0);
	str = ft_split_words(s, c, str, count);
	return (str);
}
/*
static void	ft_free(char **str)
{
	int	i;

	i = 0;
	if (!str)
		return ;
	while (str[i])
		free(str[i++]);
	free (str);
}

static int	ft_wc(char const *str, char c)
{
	int	wc;

	wc = 0;
	while (*str)
	{
		if (*str == c)
			str++;
		while (*str && *str != c)
		{
			wc++;
			str++;
		}
	}
	return (wc);
}

static void	ft_wordfill(char const *str, char c, char **splited, int wc)
{
	int	word;
	int	wl;
	int	n;

	word = 0;
	while (word < wc)
	{
		wl = 0;
		while (str && *str == c)
			str++;
		while (str && *str != c)
		{
			wl++;
			str++;
		}
		splited[word] = (char *)malloc(sizeof(char) * (wl + 1));
		if (!splited)
		{
			ft_free(splited);
			return ;
		}
		n = 0;
		while (wl)
			splited[word][n++] = *str - wl--;
		splited[word++][n] = '\0';
	}
	splited[word] = NULL;
}

char	**ft_split(char const *str, char c)
{
	char	**splited;
	int		wc;

	wc = ft_wc(str, c);
	splited = (char **)malloc(sizeof(char *) * (wc + 1));
	if (!splited)
		return (NULL);
	ft_wordfill(str, c, splited, wc);
	return (splited);
}
*/
