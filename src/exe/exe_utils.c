/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:38:43 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/04 14:22:57 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	reset_signals_for_child();
	if (setup_redirections(cmd) != 0)
	{
		rl_clear_history();
		free_shell(shell);
		exit(1);
	}
	if (execve(path, cmd->cmd_av, shell->env) == -1)
	{
		perror("minishell: execve");
		rl_clear_history();
		free_shell(shell);
		exit(126);
	}
}

/*
** validate_executable_path - Validate if path is executable
**
** DESCRIPTION:
**   Checks if path exists, is not a directory, and has execute permissions.
**
** PARAMETERS:
**   cmd_name - Command name for error messages
**   path     - Path to validate
**
** RETURN VALUE:
**   Returns: 0 if valid, 127 if not found, 126 if directory/no permission
*/
static int	validate_executable_path(char *cmd_name, char *path)
{
	if (!path)
	{
		print_error("minishell", cmd_name, "command not found");
		return (127);
	}
	if (is_directory(path))
	{
		print_error("minishell", cmd_name, "Is a directory");
		return (126);
	}
	if (!is_executable(path))
	{
		print_error("minishell", cmd_name, "Permission denied");
		return (126);
	}
	return (0);
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
**   Returns: 127 if not found, 126 if no permission, child status otherwise
*/
int	exe_external_cmd(t_shell *shell, t_cmd *cmd)
{
	pid_t	pid;
	char	*executable_path;
	int		validation_status;

	executable_path = find_executable(shell, cmd->cmd_av[0]);
	validation_status = validate_executable_path(cmd->cmd_av[0],
			executable_path);
	if (validation_status != 0)
		return (validation_status);
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
	int		code;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
	{
		reset_signals_for_child();
		if (setup_redirections(cmd) != 0)
		{
			rl_clear_history();
			free_shell(shell);
			exit(1);
		}
		code = exe_builtin(cmd, shell);
		rl_clear_history();
		free_shell(shell);
		exit(code);
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
int	exe_redirection_only(t_shell *shell, t_cmd *cmd)
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
		{
			rl_clear_history();
			free_shell(shell);
			exit(1);
		}
		rl_clear_history();
		free_shell(shell);
		exit(0);
	}
	return (wait_and_get_status(pid));
}
