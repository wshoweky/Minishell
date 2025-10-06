#include "minishell.h"

/*
** is_non_forkable_builtin - Check if builtin must not fork
**
** DESCRIPTION:
**   Checks if a builtin command affects parent shell state.
**
** PARAMETERS:
**   cmd_name - Command name to check
**
** RETURN VALUE:
**   Returns 1 if command must not fork, 0 otherwise
*/
int	is_non_forkable_builtin(char *cmd_name)
{
	if (ft_strcmp(cmd_name, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "exit") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "export") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "unset") == 0)
		return (1);
	return (0);
}
/*
** exe_builtin_with_fork - Execute builtin with redirections in child
*/
int	exe_builtin_with_fork(t_cmd *cmd, char **env)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	else if (pid == 0)
	{
		if (setup_redirections(cmd) != 0)
			exit(1);
		exit(exe_builtin(cmd, env));
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else
			return (1);
	}
}

/*
** exe_external_cmd - Execute external program
**
** DESCRIPTION:
**   Forks and executes external program using execve.
**
** PARAMETERS:
**   arena - Memory arena
**   cmd   - Command structure
**   env   - Environment variables
**
** RETURN VALUE:
**   Returns exit status of the external command
*/
int	exe_external_cmd(t_arena *arena, t_cmd *cmd, char **env)
{
	pid_t	pid;
	int		status;
	char	*executable_path;

	executable_path = find_executable(arena, cmd->cmd_av[0], env);
	if (!executable_path)
	{
		ft_printf("minishell: %s: command not found\n", cmd->cmd_av[0]);
		return (127);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	else if (pid == 0)
	{
		if (setup_redirections(cmd) != 0)
			exit(1);
		if (execve(executable_path, cmd->cmd_av, env) == -1)
		{
			perror("minishell: execve");
			exit(126);
		}
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else
			return (1);
	}
	return (0);
}
