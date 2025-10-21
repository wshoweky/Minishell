#include "minishell.h"

/**
** free_shell - Free shell state structure and all its members
**
**   Properly deallocates all memory used by the shell state.
**
**   shell - Shell state structure to free
*/
void	free_shell(t_shell *shell)
{
	int	i;

	if (!shell)
		return ;
	if (shell->env)
	{
		i = 0;
		while (shell->env[i])
			free(shell->env[i++]);
		free(shell->env);
	}
	free(shell->cwd);
	free(shell->oldpwd);
	free(shell->home);
	free(shell->user);
	free(shell->shell_name);
	free_arena(shell->arena);
	free_list_of_vars(&shell->vars);
	free(shell);
}

void	free_list_of_vars(t_var **var)
{
	t_var	*tmp;

	if (var)
	{
		while (*var)
		{
			tmp = (*var)->next_var;
			free_1_var(var);
			*var = tmp;
		}
	}
}

void	free_1_var(t_var **var)
{
	if (*var)
	{
		if ((*var)->name)
		{
			free ((*var)->name);
			(*var)->name = NULL;
		}
		if ((*var)->value)
		{
			free ((*var)->value);
			(*var)->value = NULL;
		}
		free (*var);
		*var = NULL;
	}
}

/**
** free_partial_env - Free partially allocated environment array
**
**   Helper function for cleanup during failed initialization.
**
**   shell - Shell state structure
**   count - Number of successfully allocated entries
*/
void	free_partial_env(t_shell *shell, int count)
{
	int	i;

	i = 0;
	while (i < count)
		free(shell->env[i++]);
	free(shell->env);
}

/**
** cleanup_heredoc_files - Remove all temporary heredoc files
**
** CLEANUP STRATEGY:
** 1. Traverse all commands in command table
** 2. For each command with heredoc_filename: unlink() the file
** 3. Set filename to NULL to prevent double-cleanup
**
** WHY UNLINK: Removes file from filesystem, frees disk space
** WHEN CALLED: After command execution completes (success or failure)
**
**   cmd_table - Command table with potential heredoc files
*/
void	cleanup_heredoc_files(t_cmd_table *cmd_table)
{
	t_cmd	*current_cmd;

	if (!cmd_table)
		return ;
	current_cmd = cmd_table->list_of_cmds;
	while (current_cmd)
	{
		if (current_cmd->heredoc_filename)
		{
			/* Remove temporary file from filesystem */
			if (unlink(current_cmd->heredoc_filename) == -1)
				perror("minishell: heredoc cleanup");
			/* Prevent double-cleanup */
			current_cmd->heredoc_filename = NULL;
		}
		current_cmd = current_cmd->next_cmd;
	}
}
