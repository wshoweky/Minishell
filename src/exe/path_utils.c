#include "minishell.h"

static char	*search_in_path(t_shell *shell, char *cmd, char **path_dirs);
static char	*handle_absolute_path(t_shell *shell, char *cmd);

/*
** find_executable - Find executable path for command
**
** DESCRIPTION:
**   Searches for executable in PATH or returns absolute/relative path.
**   Uses arena allocation for memory management.
**
** PARAMETERS:
**   arena - Memory arena for allocations
**   cmd - Command name
**   env - Environment variables
**
** RETURN VALUE:
**   Returns path to executable or NULL if not found
*/
char	*find_executable(t_shell *shell, char *cmd)
{
	char	*path_env;
	char	**path_dirs;

	if (!cmd || !shell)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (handle_absolute_path(shell, cmd));
	path_env = get_shell_env_value(shell, "PATH");
	if (!path_env)
		return (NULL);
	path_dirs = ar_split(shell->arena, path_env, ':');
	if (!path_dirs)
		return (NULL);
	return (search_in_path(shell, cmd, path_dirs));
}

/*
** handle_absolute_path - Handle absolute or relative path commands
**
** PARAMETERS:
**   shell - Shell state with arena
**   cmd   - Command with path separator
**
** RETURN VALUE:
**   Returns duplicated path if file exists, NULL otherwise
**   (Permission check is done later in exe_external_cmd)
*/
static char	*handle_absolute_path(t_shell *shell, char *cmd)
{
	if (is_regular_file(cmd))
		return (ar_strdup(shell->arena, cmd));
	return (NULL);
}

/*
** search_in_path - Search for command in PATH directories
**
** DESCRIPTION:
**   Searches for command file in PATH directories.
**   Returns first match found, even if not executable.
**   Permission check is done later in exe_external_cmd.
**
** PARAMETERS:
**   shell     - Shell state with arena
**   cmd       - Command name
**   path_dirs - Array of PATH directories
**
** RETURN VALUE:
**   Returns full path if found, NULL otherwise
*/
static char	*search_in_path(t_shell *shell, char *cmd, char **path_dirs)
{
	char	*full_path;
	int		i;

	i = 0;
	while (path_dirs[i])
	{
		full_path = build_path(shell, path_dirs[i], cmd);
		if (full_path)
		{
			if (is_executable(full_path))
				return (full_path);
			if (is_regular_file(full_path))
				return (full_path);
		}
		i++;
	}
	return (NULL);
}

/*
** is_executable - Check if file is executable
**
** DESCRIPTION:
**   Checks if file exists and is executable.
**
** PARAMETERS:
**   path - Path to check
**
** RETURN VALUE:
**   Returns 1 if executable, 0 otherwise
*/
int	is_executable(char *path)
{
	t_stat	file_stat;

	if (!path)
		return (0);
	if (stat(path, &file_stat) != 0)
		return (0);
	if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR))
		return (1);
	return (0);
}

/*
** is_regular_file - Check if path is a regular file
**
** DESCRIPTION:
**   Checks if file exists and is a regular file (not directory, etc.)
**   Does NOT check execute permission.
**
** PARAMETERS:
**   path - Path to check
**
** RETURN VALUE:
**   Returns 1 if regular file exists, 0 otherwise
*/
int	is_regular_file(char *path)
{
	t_stat	file_stat;

	if (!path)
		return (0);
	if (stat(path, &file_stat) != 0)
		return (0);
	if (S_ISREG(file_stat.st_mode))
		return (1);
	return (0);
}
