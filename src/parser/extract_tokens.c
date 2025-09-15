#include "minishell.h"

char	*extract_word_token(char *input, int *i);
char	*extract_pipe_token(char *input, int *i);
char	*extract_special_token(char *input, int *i);
char	*extract_redirect_in_token(char *input, int *i);
char	*extract_redirect_out_token(char *input, int *i);
/*
** extract_word_token - Extract regular word tokens
**
** DESCRIPTION:
**   Handles extraction of regular words (commands, arguments, filenames).
**   Continues until a special character is encountered.
**
** PARAMETERS:
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**
** RETURN VALUE:
**   Returns allocated string containing the word token
*/
char	*extract_word_token(char *input, int *i)
{
	int		start;
	char	*token_value;

	start = *i;
	while (input[*i] && !is_special_char(input[*i]))
		(*i)++;
	token_value = ft_substr(input, start, *i - start);
	return (token_value);
}
/*
** extract_special_token - Main dispatcher for special tokens
**
** DESCRIPTION:
**   Determines the type of special character and calls appropriate handler.
**   This function acts as a dispatcher for all shell operators.
**
** PARAMETERS:
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**
** RETURN VALUE:
**   Returns allocated string containing the operator token or NULL
*/
char	*extract_special_token(char *input, int *i)
{
	if (input[*i] == '|')
		return (extract_pipe_token(input, i));
	else if (input[*i] == '<')
		return (extract_redirect_in_token(input, i));
	else if (input[*i] == '>')
		return (extract_redirect_out_token(input, i));
	else
		return (NULL);
}

/*
** extract_pipe_token - Extract pipe operator token
**
** DESCRIPTION:
**   Handles extraction of pipe operator |.
**
** PARAMETERS:
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**
** RETURN VALUE:
**   Returns allocated string containing "|" token
*/
char	*extract_pipe_token(char *input, int *i)
{
	char	*token_value;

	token_value = ft_substr(input, *i, 1);
	(*i)++;
	return (token_value);
}

/*
** extract_redirect_out_token - Extract output redirection tokens
**
** DESCRIPTION:
**   Handles extraction of > and >> operators.
**   Looks ahead to distinguish between single and double operators.
**
** PARAMETERS:
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**
** RETURN VALUE:
**   Returns allocated string containing ">" or ">>" token
*/
char	*extract_redirect_out_token(char *input, int *i)
{
	char	*token_value;

	if (input[*i + 1] == '>')
	{
		token_value = ft_substr(input, *i, 2);
		*i += 2;
	}
	else
	{
		token_value = ft_substr(input, *i, 1);
		(*i)++;
	}
	return (token_value);
}

/*
** extract_redirect_in_token - Extract input redirection tokens
**
** DESCRIPTION:
**   Handles extraction of < and << operators.
**   Looks ahead to distinguish between single and double operators.
**
** PARAMETERS:
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**
** RETURN VALUE:
**   Returns allocated string containing "<" or "<<" token
*/
char	*extract_redirect_in_token(char *input, int *i)
{
	char	*token_value;

	if (input[*i + 1] == '<')
	{
		token_value = ft_substr(input, *i, 2);
		*i += 2;
	}
	else
	{
		token_value = ft_substr(input, *i, 1);
		(*i)++;
	}
	return (token_value);
}