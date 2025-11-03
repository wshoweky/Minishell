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
**   Returns 0 on success, -1 on errors
*/
int	extract_special_token(t_shell *shell, char **string, char current)
{
	if (current == '|')
		return (extract_pipe_token(shell, string));
	else if (current == '<')
		return (extract_redirect_in_token(shell, string));
	else if (current == '>')
		return (extract_redirect_out_token(shell, string));
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
**   Returns 0 on success, -1 on errors, 2 for syntax error
*/
int	extract_pipe_token(t_shell *shell, char **string)
{
	if (*string != NULL)
	{
		shell->last_exit_status = 2;
		return (err_msg_n_return_value("Syntax error near |\n", 2));
	}
	*string = ar_strdup(shell->arena, "|");
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
**   Returns 0 on success, -1 on errors, 2 for syntax error
*/
int	extract_redirect_in_token(t_shell *shell, char **string)
{
	if (!*string)
	{
		*string = ar_strdup(shell->arena, "<");
		if (!*string)
			return (err_msg_n_return_value("strdup failed for <\n", -1));
		return (0);
	}
	else if (*string && !ft_strcmp(*string, "<"))
	{
		*string = ar_add_char_to_str(shell->arena, *string, '<');
		if (!*string)
			return (err_msg_n_return_value("Failed to add <\n", -1));
		return (0);
	}
	else
	{
		shell->last_exit_status = 2;
		return (err_msg_n_return_value("Syntax error near <\n", 2));
	}
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
**   Returns 0 on success, -1 on errors, 2 for syntax error
*/
int	extract_redirect_out_token(t_shell *shell, char **string)
{
	if (!*string)
	{
		*string = ar_strdup(shell->arena, ">");
		if (!*string)
			return (err_msg_n_return_value("strdup failed for >\n", -1));
		return (0);
	}
	else if (*string && !ft_strcmp(*string, ">"))
	{
		*string = ar_add_char_to_str(shell->arena, *string, '>');
		if (!*string)
			return (err_msg_n_return_value("Failed to add >\n", -1));
		return (0);
	}
	else
	{
		shell->last_exit_status = 2;
		return (err_msg_n_return_value("Syntax error near >\n", 2));
	}
}
