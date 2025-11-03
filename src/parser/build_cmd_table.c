#include "minishell.h"

// static void    print_cmd_table(t_cmd_table *table) //for debugging purpose
// {
// 	if (!table)
// 	{
// 		ft_printf("Command table not found\n");
// 		return ;
// 	}
// 	ft_printf("\nPrinting command table\n");
// 	ft_printf("Number of commands: %i\n", table->cmd_count);
// 	t_cmd *current_cmd = table->list_of_cmds;
// 	int cmd_index = 1;
	
// 	while (current_cmd)
// 	{
// 		printf("------------\n");
// 		printf("COMMAND #%d:\n", cmd_index);
		
// 		// Print arguments
// 		printf("cmd_av: [");
// 		if (current_cmd->cmd_av)
// 		{
// 			for (int i = 0; current_cmd->cmd_av[i] != NULL; i++)
// 			{
// 				printf("\"%s\"", current_cmd->cmd_av[i]);
// 				if (current_cmd->cmd_av[i + 1] != NULL)
// 					printf(", ");
// 			}
// 		}
// 		printf("]\n");
// 		if (current_cmd->redirections)
// 		{
// 			t_redir *redir_ptr = current_cmd->redirections;  // Use local pointer!
// 			while (redir_ptr)  // Traverse without modifying original
// 			{
// 				printf("Redirection: %s\n", get_token_type_name(redir_ptr->tok_type));
// 				printf("filename saved: %s\n", redir_ptr->filename);
// 				redir_ptr = redir_ptr->next;  // Move local pointer
// 			}
// 		}
// 		current_cmd = current_cmd->next_cmd;
// 		cmd_index++;
// 	}
// }


/* Parses a linked list of tokens into a command table structure.

Builds a command table where:
- Pipes (|) separate commands and increment the command count
- Redirections (<, >, >>, <<) set redirection type and capture filename
- Regular word tokens are added as command arguments
*/
int	register_to_table(t_shell *shell, t_tokens *list_of_toks,
				t_cmd_table *table)
{
	t_tokens	*current_tok;
	t_cmd		*current_cmd;
	int			check;

	if (list_of_toks == NULL)
		return (-1);
	current_tok = list_of_toks;
	current_cmd = new_cmd_alloc(shell->arena);
	if (!current_cmd)
		return (err_msg_n_return_value("Memory alloc failed for t_cmd\n", -1));
	table->list_of_cmds = current_cmd;
	table->cmd_count = 1;
	while (current_tok)
	{
		check = check_current_token(shell, current_tok, &current_cmd, table);
		if (check == -1 || check == 2)
		{
			table = NULL;
			return (check);
		}
		current_tok = current_tok->next;
	}
	// print_cmd_table(table); ft_printf ("------------\n\n");
	return (0);
}

/* Create space with ar_alloc() for a t_cmd struct
*/
t_cmd	*new_cmd_alloc(t_arena *arena)
{
	t_cmd	*new;

	new = ar_alloc(arena, sizeof(t_cmd));
	if (!new)
		return (NULL);
	new->cmd_av = NULL;
	new->redirections = NULL;
	new->heredoc_filename = NULL;
	new->next_cmd = NULL;
	return (new);
}

/* Process tokens for building command table
- Pipes: validate syntax then create a new t_cmd struct
- Redirections: validate file name, then create redirection expansion
- Words: pass to helper function for further processing

Return: 0 on success, -1 on errors
*/
int	check_current_token(t_shell *shell, t_tokens *token, t_cmd **current_cmd,
	t_cmd_table *table)
{
	if (token->type == TOKEN_PIPE)
	{
		if (!((*current_cmd)->cmd_av || (*current_cmd)->redirections)
			|| !token->next)
			return (err_msg_n_return_value("Syntax error around pipe\n", 2));
		(*current_cmd)->next_cmd = new_cmd_alloc(shell->arena);
		if (!(*current_cmd)->next_cmd)
			return (err_msg_n_return_value("Memory allocation failed for "
					"new command\n", -1));
		*current_cmd = (*current_cmd)->next_cmd;
		table->cmd_count++;
		return (0);
	}
	else if (is_redirection(token->type))
	{
		if (!token->next || token->next->type != TOKEN_WORD)
			return (err_msg_n_return_value("Syntax error around redirection\n",
					2));
		if (make_redir(shell, token, *current_cmd) == -1)
			return (-1);
		return (0);
	}
	else
		return (check_token_word(shell, token, *current_cmd));
}

void	*err_msg_n_return_null(char *msg)
{
	if (msg)
		ft_putstr_fd(msg, 2);
	return (NULL);
}

int	err_msg_n_return_value(char *msg, int value)
{
	if (msg)
		ft_putstr_fd(msg, 2);
	return (value);
}
