#include "minishell.h"

/**
** get_shell_env_value - Get environment variable value from shell
**
**   Searches shell's environment array for variable and returns its value.
**
**   shell - Shell state structure
**   name  - Variable name to search for
**
**   Returns: Pointer to value string, or NULL if not found
*/
char	*get_shell_env_value(t_shell *shell, char *name)
{
	int	i;
	int	name_len;

	if (!shell || !shell->env || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0
			&& shell->env[i][name_len] == '=')
			return (&shell->env[i][name_len + 1]);
		i++;
	}
	return (NULL);
}

/**
** set_shell_env_value - Set environment variable in shell
**
**   Sets or updates an environment variable in the shell's environment.
**   Automatically resizes the environment array if needed.
**
**   shell - Shell state structure
**   name  - Variable name
**   value - Variable value
**
**   Returns: 1 on success, 0 on failure
*/
int	set_shell_env_value(t_shell *shell, char *name, char *value)
{
	char	*new_var;
	int		i;

	if (!shell || !name || !value)
		return (0);
	new_var = create_env_string(name, value);
	if (!new_var)
		return (0);
	i = find_env_index(shell, name);
	if (i >= 0)
	{
		free(shell->env[i]);
		shell->env[i] = new_var;
		return (1);
	}
	if (!resize_env_if_needed(shell))
		return (free(new_var), 0);
	shell->env[shell->env_count] = new_var;
	shell->env[++shell->env_count] = NULL;
	return (1);
}

/**
** unset_shell_env_value - Remove environment variable from shell
**
**   Removes an environment variable from the shell's environment.
**	Shifts remaining variables to fill the gap and updates count.
**
**   shell - Shell state structure
**   name  - Variable name to remove
**
**   Returns: 1 on success, 0 if variable not found, -1 on errors
*/
int	unset_shell_env_value(t_shell *shell, char *name)
{
	int	i;
	int	found_index;

	if (!shell || !name)
		return (-1);
	found_index = find_env_index(shell, name);
	if (found_index == -1)
		return (-1);
	else if (found_index == -2)
		return (0);
	free(shell->env[found_index]);
	i = found_index;
	while (i < shell->env_count - 1)
	{
		shell->env[i] = shell->env[i + 1];
		i++;
	}
	// pre-decrement count and NULL-terminate the array!
	shell->env[--shell->env_count] = NULL;
	return (1);
}
