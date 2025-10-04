/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_break_input.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 16:59:39 by gita              #+#    #+#             */
/*   Updated: 2025/10/04 21:27:36 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Process input string to extract token value
- Group everything inside quotes to the same string
- Outside of quotes, handle if there is special operators or just words

 Return: 0 on success, -1 on errors
*/
int	chop_up_input(t_arena *arena, char *input, int *i, char **string)
{
	int	in_quotes;
	int	need_break;

	in_quotes = 0;
	while (input[*i])
	{
		if (in_quotes)
		{
			if (char_in_quotes(arena, string, input[*i], &in_quotes) == -1)
				return (-1);
		}
		else
		{
			need_break = char_outside_quotes(arena, string, input[*i],
					&in_quotes);
			if (need_break == -1)
				return (-1);
			if (need_break == 1)
				break ;
		}
		(*i)++;
	}
	return (0);
}

/* Keep adding characters to the current string until seeing the closing quote
 Return: 0 on success, -1 on errors
*/
int	char_in_quotes(t_arena *arena, char **string, char current_char,
	int *in_quotes)
{
	*string = ar_add_char_to_str(arena, *string, current_char);
	if (!*string)
		return (-1);
	if ((current_char == '"' && *in_quotes == 2)
		|| (current_char == '\'' && *in_quotes == 1))
		*in_quotes = 0;
	return (0);
}

/*
- Handle operators (|, >, <, >>, <<) for special tokens
- Enter quote mode when seeing a quote
- Break the string value when seeing whitespace

 Return: 1 to break token, 0 to continue building string, -1 on errors
*/
int	char_outside_quotes(t_arena *arena, char **string, char current_char,
	int *in_quotes)
{
	int	check;

	if (current_char == '|' || current_char == '<' || current_char == '>')
	{
		if (*string && ft_strcmp(*string, "<") && ft_strcmp(*string, ">"))
			return (1);
		else
			return (extract_special_token(arena, string, current_char));
	}
	else if (current_char == '"' || current_char == '\'')
	{
		if (char_is_quote(arena, string, current_char, in_quotes) == -1)
			return (-1);
	}
	else if (current_char == ' ' || current_char == '\t'
		|| current_char == '\n')
		return (1);
	else
	{
		check = char_normal_outside_quotes(arena, string, current_char);
		if (check != 0)
			return (check);
	}
	return (0);
}

/* When not in quote mode and seeing a quote:
- Set in_quotes value to indicate whether in double or single quote
- Add the quote to the string

 Return: 0 on success, -1 on errors
*/
int	char_is_quote(t_arena *arena, char **string, char current_char,
		int *in_quotes)
{
	if (current_char == '"')
		*in_quotes = 2;
	else if (current_char == '\'')
		*in_quotes = 1;
	*string = ar_add_char_to_str(arena, *string, current_char);
	if (!*string)
		return (err_msg_n_return_value("Memory alloc fail for quote\n", -1));
	return (0);
}

/* For other characters than operator/quote/whitespace outside of quotes
- Check if needed to break to build special tokens
- If not, add the character to current string

 Return: 1 to break token, 0 to continue building string, -1 on errors
*/
int	char_normal_outside_quotes(t_arena *arena, char **string, char current_char)
{
	if (*string && (!ft_strcmp(*string, "|") || !ft_strcmp(*string, ">")
			|| !ft_strcmp(*string, ">>") || !ft_strcmp(*string, "<")
			|| !ft_strcmp(*string, "<<")))
		return (1);
	*string = ar_add_char_to_str(arena, *string, current_char);
	if (!*string)
		return (err_msg_n_return_value("Failed to add char to string\n", -1));
	return (0);
}
