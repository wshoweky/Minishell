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
- Otherwise, add characters to the string with helper function
- Ensure output is not NULL with helper function

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
			if (build_output(arena, output, str[*i]) == -1)
				return (-1);			
		}
		(*i)++;
	}
	if (ensure_output_not_null(arena, output) == -1)
		return (-1);
	return (0);
}

/*For characters that are not handled previously, add them to output string
Return: 0 on success, -1 on error
(helper function of remove_quotes_for_plain_string())
*/
int	build_output(t_arena *arena, char **output, char c)
{
	*output = ar_add_char_to_str(arena, *output, c);
	if (!*output)
		return (err_msg_n_return_value("Failed to unquote string\n", -1));
	return (0);
}

/*If output is still NULL after going thru the input string, make output
an empty string instead
Return: 0 on success, -1 on error
(helper function of remove_quotes_for_plain_string())
*/
int	ensure_output_not_null(t_arena *arena, char **output)
{
	if (!*output)
	{
		*output = ar_alloc(arena, 1);
		if (!*output)
			return (err_msg_n_return_value("Failed to allocate empty string\n",
				-1));
		(*output)[0] = '\0';
	}
	return (0);
}
