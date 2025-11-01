#include "minishell.h"

/**
** create_env_string - Create environment string from name and value
**
**   Creates a "NAME=value" string for environment array.
**
**   name  - Variable name
**   value - Variable value
**
**   Returns: Allocated string, or NULL on failure
*/
char	*create_env_string(char *name, char *value)
{
	char	*result;
	int		name_len;
	int		value_len;

	name_len = ft_strlen(name);
	value_len = ft_strlen(value);
	result = malloc(name_len + value_len + 2);
	if (!result)
		return (NULL);
	ft_strlcpy(result, name, name_len + 1);
	result[name_len] = '=';
	ft_strlcpy(result + name_len + 1, value, value_len + 1);
	return (result);
}

/**
** find_env_index - Find index of environment variable
**
**   Searches for environment variable and returns its index.
**
**   shell - Shell state structure
**   name  - Variable name to find
**
**   Returns: Index if found, -1 if errors, -2 if not found
*/
int	find_env_index(t_shell *shell, char *name)
{
	int	i;
	int	name_len;

	if (!shell || !name)
		return (-1);
	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0
			&& shell->env[i][name_len] == '=')
			return (i);
		i++;
	}
	return (-2);
}

/**
** resize_env_if_needed - Resize environment array if needed
**
**   Expands the environment array if it's getting full.
**
**   shell - Shell state structure
**
**   Returns: 1 on success, 0 on failure
*/
int	resize_env_if_needed(t_shell *shell)
{
	char	**new_env;
	int		i;

	if (shell->env_count + 1 < shell->env_capacity)
		return (1);
	shell->env_capacity *= 2;
	new_env = malloc(sizeof(char *) * (shell->env_capacity + 1));
	if (!new_env)
		return (0);
	i = 0;
	while (i < shell->env_count)
	{
		new_env[i] = shell->env[i];
		i++;
	}
	new_env[i] = NULL;
	free(shell->env);
	shell->env = new_env;
	return (1);
}

/**
** update_shell_cwd - Update current working directory
**
**   Updates the shell's CWD and PWD environment variable.
**
**   shell - Shell state structure
**
**   Returns: 1 on success, 0 on failure
*/
int	update_shell_cwd(t_shell *shell, char *old_dir)
{
	char	*new_cwd;
	char	*old_cwd;

	if (!shell)
		return (0);
	new_cwd = getcwd(NULL, 0);
	if (!new_cwd)
		return (0);
	old_cwd = shell->cwd;
	shell->cwd = new_cwd;
	if (old_dir && !set_shell_env_value(shell, "OLDPWD", old_dir))
	{
		shell->cwd = old_cwd;
		free(new_cwd);
		return (0);
	}
	if (!set_shell_env_value(shell, "PWD", shell->cwd))
	{
		shell->cwd = old_cwd;
		free(new_cwd);
		return (0);
	}
	free(old_cwd);
	return (1);
}

/*
** wait_and_get_status - Wait for child and extract exit status
**
** DESCRIPTION:
**   Waits for child process and extracts exit status.
**   Handles both normal exit and signal termination.
**
** PARAMETERS:
**   pid - Child process ID
**
** RETURN VALUE:
**   Returns child's exit status
*/
int	wait_and_get_status(pid_t pid)
{
	int	status;

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
