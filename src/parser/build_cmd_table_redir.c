#include "minishell.h"

/* Check if token is a redirectional token.
*/
int	is_redirection(t_token_type check)
{
	if (check == TOKEN_REDIRECT_IN || check == TOKEN_REDIRECT_OUT
		|| check == TOKEN_APPEND || check == TOKEN_HEREDOC)
		return (8);
	return (0);
}

/* Create and append a new redirection node to the command

- Allocate memory for a new t_redir structure
- Set redirection type from current token
- Move to next token and ar_strdup the filename
- Appends the new t_redir to the command's redirection list.
	If no redirection exists, it becomes the first node; otherwise at the end.

Return: 0 on success, -1 on errors
*/
int	make_redir(t_shell *shell, t_tokens *curr_tok, t_cmd *curr_cmd)
{
	t_redir	*new;
	t_redir	*find_tail;

	new = ar_alloc(shell->arena, sizeof(t_redir));
	if (!new)
		return (err_msg_n_return_value("Mem alloc failed for t_redir\n", -1));
	set_redir_type(curr_tok->type, &new->tok_type);
	new->next = NULL;
	*curr_tok = *curr_tok->next;
	if (work_on_filename(shell, curr_tok, &new->filename) == -1)
		return (-1);
	if (!curr_cmd->redirections)
		curr_cmd->redirections = new;
	else
	{
		find_tail = curr_cmd->redirections;
		while (find_tail->next)
			find_tail = find_tail->next;
		find_tail->next = new;
	}
	return (0);
}

/* Set the redirectional type to be the same with the token type
(helper function of make_redir())
*/
void	set_redir_type(t_token_type tok_type, t_token_type *redir_type)
{
	if (tok_type == TOKEN_REDIRECT_IN)
		*redir_type = TOKEN_REDIRECT_IN;
	else if (tok_type == TOKEN_REDIRECT_OUT)
		*redir_type = TOKEN_REDIRECT_OUT;
	else if (tok_type == TOKEN_APPEND)
		*redir_type = TOKEN_APPEND;
	else if (tok_type == TOKEN_HEREDOC)
		*redir_type = TOKEN_HEREDOC;
}

/* Copy the string of the token to be the file name. Expand variables if any.
Return: 0 on success, -1 on errors
*/
int	work_on_filename(t_shell *shell, t_tokens *tok_name, char **name)
{
	if (ft_strchr(tok_name->value, '$') || ft_strchr(tok_name->value, '&'))
	{
		if (expand_variable_name(shell, &tok_name->value, 1) == -1)
			return (-1);
	}
	if (tok_name->value[0] == 0)
		return (0);
	*name = ar_strdup(shell->arena, tok_name->value);
	if (!*name)
		return (err_msg_n_return_value("strdup failed for redir filename\n",
				-1));
	return (0);
}
