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
	shell->env[--shell->env_count] = NULL;
	return (1);
}

/*
** build_path - Build full path from directory and filename
**
** DESCRIPTION:
**   Concatenates directory path with filename using '/'.
**   Uses arena allocation for memory management.
**
** PARAMETERS:
**   arena - Memory arena for allocations
**   dir  - Directory path
**   file - Filename
**
** RETURN VALUE:
**   Returns allocated full path or NULL on error
*/
char	*build_path(t_shell *shell, char *dir, char *file)
{
	char	*dir_with_slash;
	char	*path;
	int		dir_len;

	if (!dir || !file || !shell)
		return (NULL);
	dir_len = ft_strlen(dir);
	if (dir_len > 0 && dir[dir_len - 1] != '/')
	{
		dir_with_slash = ar_strjoin(shell->arena, dir, "/");
		if (!dir_with_slash)
			return (NULL);
		path = ar_strjoin(shell->arena, dir_with_slash, file);
	}
	else
		path = ar_strjoin(shell->arena, dir, file);
	return (path);
}

/*
** print_error - Print error message to stderr
**
**   Prints formatted error message to stderr (fd 2) so it's visible
**   in pipelines instead of being piped to the next command.
**
**   prefix - Error prefix (e.g., "minishell")
**   cmd    - Command name that caused error
**   msg    - Error message
*/
void	print_error(char *prefix, char *cmd, char *msg)
{
	if (prefix)
	{
		ft_putstr_fd(prefix, 2);
		ft_putstr_fd(": ", 2);
	}
	if (cmd)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
	}
	if (msg)
	{
		ft_putstr_fd(msg, 2);
		ft_putstr_fd("\n", 2);
	}
}
