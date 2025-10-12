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
** execute_child_process - Execute command in child process
**
** DESCRIPTION:
**   Child process: setup redirections and execute via execve.
**   Never returns - exits with appropriate status.
**
** PARAMETERS:
**   shell - Shell state structure
**   cmd   - Command structure
**   path  - Executable path
*/
void	execute_child_process(t_shell *shell, t_cmd *cmd, char *path)
{
	if (setup_redirections(cmd) != 0)
		exit(1);
	if (execve(path, cmd->cmd_av, shell->env) == -1)
	{
		perror("minishell: execve");
		exit(126);
	}
}

/*
** exe_external_cmd - Execute external program
**
** DESCRIPTION:
**   Finds executable, forks, and executes external program.
**
** PARAMETERS:
**   shell - Shell state structure
**   cmd   - Command structure
**
** RETURN VALUE:
**   Returns exit status: 127 if not found, child status otherwise
*/
int	exe_external_cmd(t_shell *shell, t_cmd *cmd)
{
	pid_t	pid;
	char	*executable_path;

	executable_path = find_executable(shell, cmd->cmd_av[0]);
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
	if (pid == 0)
		execute_child_process(shell, cmd, executable_path);
	return (wait_and_get_status(pid));
}

/*
** exe_builtin_with_fork - Execute builtin with redirections in child
**
** DESCRIPTION:
**   Forks and executes builtin in child process with redirections.
**   Used for forkable builtins (echo, pwd, env) with redirections.
**
** PARAMETERS:
**   cmd   - Command structure
**   shell - Shell state structure
**
** RETURN VALUE:
**   Returns exit status of builtin command
*/
int	exe_builtin_with_fork(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
	{
		if (setup_redirections(cmd) != 0)
			exit(1);
		exit(exe_builtin(cmd, shell));
	}
	return (wait_and_get_status(pid));
}

/*
** exe_redirection_only - Handle commands with only redirections
**
** DESCRIPTION:
**   Processes redirections without executing a command (e.g., "< in > out").
**   Fork required since redirections may fail and shouldn't affect parent.
**
** PARAMETERS:
**   cmd - Command structure with redirections
**
** RETURN VALUE:
**   Returns 0 on success, 1 on failure
*/
int	exe_redirection_only(t_cmd *cmd)
{
	pid_t	pid;

	if (!cmd->redirections)
		return (0);
	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
	{
		if (setup_redirections(cmd) != 0)
			exit(1);
		exit(0);
	}
	return (wait_and_get_status(pid));
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
	return (1);
}
