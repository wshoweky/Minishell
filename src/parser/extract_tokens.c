#include "minishell.h"

/*
** extract_special_token - Main dispatcher for special tokens
**
** DESCRIPTION:
**   Determines the type of special character and calls appropriate handler.
**   This function acts as a dispatcher for all shell operators.
**
** PARAMETERS:
**   arena   - Memory arena for allocations
**   string  - Pointer to string pointer (modified by reference)
**   current - Current character being processed
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	extract_special_token(t_arena *arena, char **string, char current)
{
	if (current == '|')
		return (extract_pipe_token(arena, string));
	else if (current == '<')
		return (extract_redirect_in_token(arena, string));
	else if (current == '>')
		return (extract_redirect_out_token(arena, string));
	else
		return (err_msg_n_return_value("Not one of special tokens\n", -1));
}

/*
** extract_pipe_token - Extract pipe operator token
**
** DESCRIPTION:
**   Handles extraction of pipe operator |.
**   Ensures it is an empty string and sets it to "|"
**
** PARAMETERS:
**   arena  - Memory arena for allocations
**   string - Pointer to string pointer (modified by reference)
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	extract_pipe_token(t_arena *arena, char **string)
{
	if (*string != NULL)
		return (err_msg_n_return_value("String not empty for |\n", -1));
	*string = ar_strdup(arena, "|");
	if (!*string)
		return (err_msg_n_return_value("strdup failed for |\n", -1));
	return (0);
}

/*
** extract_redirect_in_token - Extract input redirection tokens
**
** DESCRIPTION:
**   Handles extraction of < and << operators.
**   Builds single or double operator based on existing string content.
**
** PARAMETERS:
**   arena  - Memory arena for allocations
**   string - Pointer to string pointer (modified by reference)
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	extract_redirect_in_token(t_arena *arena, char **string)
{
	if (!*string)
	{
		*string = ar_strdup(arena, "<");
		if (!*string)
			return (err_msg_n_return_value("strdup failed for <\n", -1));
		return (0);
	}
	else if (*string && !ft_strcmp(*string, "<"))
	{
		*string = ar_add_char_to_str(arena, *string, '<');
		if (!*string)
			return (err_msg_n_return_value("Failed to add <\n", -1));
		return (0);
	}
	else
		return (err_msg_n_return_value("String has different character \
			than <\n", -1));
}

/*
** extract_redirect_out_token - Extract output redirection tokens
**
** DESCRIPTION:
**   Handles extraction of > and >> operators.
**   Builds single or double operator based on existing string content.
**
** PARAMETERS:
**   arena  - Memory arena for allocations
**   string - Pointer to string pointer (modified by reference)
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	extract_redirect_out_token(t_arena *arena, char **string)
{
	if (!*string)
	{
		*string = ar_strdup(arena, ">");
		if (!*string)
			return (err_msg_n_return_value("strdup failed for >\n", -1));
		return (0);
	}
	else if (*string && !ft_strcmp(*string, ">"))
	{
		*string = ar_add_char_to_str(arena, *string, '>');
		if (!*string)
			return (err_msg_n_return_value("Failed to add >\n", -1));
		return (0);
	}
	else
		return (err_msg_n_return_value("String has different character \
			than >\n", -1));
}
