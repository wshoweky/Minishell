/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_quote_check.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 16:53:31 by gita              #+#    #+#             */
/*   Updated: 2025/10/08 11:32:21 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Check for quotes in the string
- Return NULL if there is unclosed quote
- If no $ is present in the string, remove quotes (that are outside of quotes)
and return a clean string, or NULL if errors
- If there is $, return the string as it is
*/
char	*check_for_quoted_string(t_arena *arena, char *str)
{
	char	*output;
	size_t	i;

	if ((ft_strcmp(str, "|") == 0) || (ft_strcmp(str, ">") == 0)
		|| (ft_strcmp(str, ">>") == 0) || (ft_strcmp(str, "<") == 0)
		|| (ft_strcmp(str, "<<") == 0))
		return (str);
	output = NULL;
	i = 0;
	if (check_for_unclosed_quote(str, &i) == 1)
		return (err_msg_n_return_null("Unclosed quote\n"));
	if (!ft_strchr(str, '$') && !ft_strchr(str, '&'))
	{
		if (remove_quotes_for_plain_string(arena, str, &output, &i) == -1)
			return (NULL);
		else
			return (output);
	}
	else
		return (str);
}

/*Track quote state while advancing index
Return: 1 if unclosed quote found, 0 if all quotes are properly closed
(helper function of check_for_quoted_string())
*/
int	check_for_unclosed_quote(char *str, size_t *i)
{
	char	quote;
	int		in_quote;

	in_quote = 0;
	quote = 0;
	while (str[*i])
	{
		if (!in_quote && (str[*i] == '"' || str[*i] == '\''))
		{
			quote = str[*i];
			in_quote = 1;
		}
		else if (in_quote && str[*i] == quote)
			in_quote = 0;
		(*i)++;
	}
	if (in_quote)
		return (1);
	return (0);
}

/*
- Upon seeing a quote, enter quote mode and skip the quote
- Only exit quote mode and skip when seeing closing quote
- Otherwise, add characters to the string

 Return: 0 on success, -1 on errors
*/
int	remove_quotes_for_plain_string(t_arena *arena, char *str, char **output,
		size_t *i)
{
	char	quote;
	int		in_quote;

	*i = 0;
	in_quote = 0;
	quote = 0;
	while (str[*i])
	{
		if (!in_quote && (str[*i] == '"' || str[*i] == '\''))
		{
			quote = str[*i];
			in_quote = 1;
		}
		else if (in_quote && str[*i] == quote)
			in_quote = 0;
		else
		{
			*output = ar_add_char_to_str(arena, *output, str[*i]);
			if (!*output)
				return (err_msg_n_return_value("Failed making string \
					not quoted anymore\n", -1));
		}
		(*i)++;
	}
	//fix: If output is still NULL (empty quoted string), create empty string
	if (!*output)
	{
		*output = ar_alloc(arena, 1);
		if (!*output)
			return (err_msg_n_return_value("Failed to allocate empty string\n", -1));
		(*output)[0] = '\0';
	}
	return (0);
}
