//#include "../include/exe.h"
#include "minishell.h"

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
char	*find_executable(t_arena *arena, char *cmd, char **env)
{
	char	*path_env;
	char	**path_dirs;
	char	*full_path;
	int		i;

	if (!cmd || !arena)
		return (NULL);
	
	// If command contains '/' it's a path (absolute or relative)
	if (ft_strchr(cmd, '/'))
	{
		if (is_executable(cmd))
			return (ar_strdup(arena, cmd));
		return (NULL);
	}
	
	// Search in PATH
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	
	path_dirs = ar_split(arena, path_env, ':');
	if (!path_dirs)
		return (NULL);
	
	i = 0;
	while (path_dirs[i])
	{
		full_path = build_path(arena, path_dirs[i], cmd);
		if (full_path && is_executable(full_path))
		{
			// No need to free path_dirs - it's in arena
			return (full_path);
		}
		// No need to free full_path - it's in arena
		i++;
	}
	
	// No need to free path_dirs - it's in arena
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

	// Basic input validation - null path is not executable
	if (!path)
		return (0);
	
	// Use stat() system call to get file metadata
	// stat() fills the file_stat structure with file information
	// Returns 0 on success, -1 on failure (file doesn't exist, no permissions, etc.)
	if (stat(path, &file_stat) != 0)
		return (0);
	
	// Check two conditions for executability:
	// 1. S_ISREG(file_stat.st_mode): Is it a regular file? (not directory, symlink, etc.)
	//    - S_ISREG is a macro that checks file type bits in st_mode
	//    - st_mode contains both file type and permission information
	// 2. (file_stat.st_mode & S_IXUSR): Does the user (owner) have execute permission?
	//    - S_IXUSR is the "user execute" permission bit (octal 0100)
	//    - Bitwise AND (&) checks if this specific bit is set
	//    - Only checks owner permissions, not group or others
	if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR))
		return (1);
	
	// File exists but is either not a regular file or not executable by owner
	return (0);
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
char	*build_path(t_arena *arena, char *dir, char *file)
{
	char	*dir_with_slash;
	char	*path;
	int		dir_len;

	if (!dir || !file || !arena)
		return (NULL);
	
	dir_len = ft_strlen(dir);
	
	// Add slash if needed
	if (dir_len > 0 && dir[dir_len - 1] != '/')
	{
		dir_with_slash = ar_strjoin(arena, dir, "/");
		if (!dir_with_slash)
			return (NULL);
		path = ar_strjoin(arena, dir_with_slash, file);
		// No need to free dir_with_slash - it's in arena
	}
	else
		path = ar_strjoin(arena, dir, file);
	
	return (path);
}

/*
** get_env_value - Get environment variable value
**
** DESCRIPTION:
**   Searches environment array for variable and returns its value.
**
** PARAMETERS:
**   env  - Environment variables array
**   name - Variable name to find
**
** RETURN VALUE:
**   Returns value string or NULL if not found
*/
char	*get_env_value(char **env, char *name)
{
	int		i;
	int		name_len;

	if (!env || !name)
		return (NULL);
	
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (&env[i][name_len + 1]);
		i++;
	}
	
	return (NULL);
}
