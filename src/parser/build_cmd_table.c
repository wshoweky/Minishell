/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_cmd_table.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 15:51:21 by gita              #+#    #+#             */
/*   Updated: 2025/10/04 20:16:22 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void    print_cmd_table(t_cmd_table *table) //for debugging purpose
{
	if (!table)
	{
		ft_printf("Command table not found\n");
		return ;
	}
	ft_printf("\nPrinting command table\n");
	ft_printf("Number of commands: %i\n", table->cmd_count);
	t_cmd *current_cmd = table->list_of_cmds;
	int cmd_index = 1;
	
	while (current_cmd)
	{
		printf("------------\n");
		printf("COMMAND #%d:\n", cmd_index);
		
		// Print arguments
		printf("cmd_av: [");
		if (current_cmd->cmd_av)
		{
			for (int i = 0; current_cmd->cmd_av[i] != NULL; i++)
			{
				printf("\"%s\"", current_cmd->cmd_av[i]);
				if (current_cmd->cmd_av[i + 1] != NULL)
					printf(", ");
			}
		}
		printf("]\n");
		if (current_cmd->redirections)
		{
			printf("Redirection: %s\n", get_token_type_name(current_cmd->redirections->tok_type));
			printf("filename saved: %s\n", current_cmd->redirections->filename);
			while (current_cmd->redirections->next)
			{
				current_cmd->redirections = current_cmd->redirections->next;
				printf("Redirection: %s\n", get_token_type_name(current_cmd->redirections->tok_type));
				printf("filename saved: %s\n", current_cmd->redirections->filename);
			}
		}
		current_cmd = current_cmd->next_cmd;
		cmd_index++;
	}
}


/* Parses a linked list of tokens into a command table structure.

Builds a command table where:
- Pipes (|) separate commands and increment the command count
- Redirections (<, >, >>, <<) set redirection type and capture filename
- Regular word tokens are added as command arguments
*/
t_cmd_table	*register_to_table(t_arena *arena, t_tokens *list_of_toks)
{
	t_cmd_table	*table;
	t_tokens	*current_tok;
	t_cmd		*current_cmd;

	if (list_of_toks == NULL)
		return (NULL);
	current_tok = list_of_toks;
	table = ar_alloc(arena, sizeof(t_cmd_table));
	if (!table)
		return (err_msg_n_return_null("Memory alloc failed for t_cmd_table\n"));
	current_cmd = new_cmd_alloc(arena);
	if (!current_cmd)
		return (err_msg_n_return_null("Memory alloc failed for t_cmd\n"));
	table->list_of_cmds = current_cmd;
	table->cmd_count = 1;
	while (current_tok)
	{
		if (check_current_token(arena, current_tok, &current_cmd, table) == -1)
			return (NULL);
		current_tok = current_tok->next;
	}
	print_cmd_table(table); ft_printf ("------------\n\n");
	return (table);
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
	new->next_cmd = NULL;
	return (new);
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
