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
	free(shell);
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

