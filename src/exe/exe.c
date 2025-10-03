/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 17:48:17 by wshoweky          #+#    #+#             */
/*   Updated: 2025/10/03 17:48:36 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../include/exe.h"
#include "minishell.h"

/*
** execute_command - Main command execution dispatcher
**
** DESCRIPTION:
**   Decides whether to execute as built-in or external command.
**   This is the main entry point for command execution.
**
** PARAMETERS:
**   cmd_table - Command table from parser
**   env       - Environment variables
**
** RETURN VALUE:
**   Returns exit status of executed command(s)
*/
int	exe_cmd(t_arena *arena, t_cmd_table *cmd_table, char **env)
{
	t_cmd	*current_cmd;
	int		exit_status;

	if (!cmd_table || !cmd_table->list_of_cmds || !arena)
		return (0);
	current_cmd = cmd_table->list_of_cmds;
	exit_status = 0;
	// For now, handle single commands only
	if (cmd_table->cmd_count == 1)
		exit_status = exe_single_cmd(arena, current_cmd, env);
	else
	{
		ft_printf("Pipeline execution not yet implemented\n");
		exit_status = 1;
	}
	return (exit_status);
}
/*
** execute_builtin - Execute built-in command
**
** DESCRIPTION:
**   Dispatches to appropriate built-in command handler.
**
** PARAMETERS:
**   cmd - Command structure
**   env - Environment variables
**
** RETURN VALUE:
**   Returns exit status of built-in command
*/
int	exe_builtin(t_cmd *cmd, char **env)
{
	char	*cmd_name;

	if (!cmd || !cmd->cmd_av || !cmd->cmd_av[0])
		return (0);
	cmd_name = cmd->cmd_av[0];
	if (ft_strcmp(cmd_name, "cd") == 0)
		return (builtin_cd(cmd));
	else if (ft_strcmp(cmd_name, "echo") == 0)
		return (builtin_echo(cmd));
	else if (ft_strcmp(cmd_name, "pwd") == 0)
		return (builtin_pwd(cmd));
	else if (ft_strcmp(cmd_name, "env") == 0)
		return (builtin_env(env));
	else if (ft_strcmp(cmd_name, "exit") == 0)
		return (builtin_exit(cmd));
	// TODO implement export and unset
	ft_printf("Built-in '%s' not yet implemented\n", cmd_name);
	return (1);
}
/*
** is_builtin - Check if command is a built-in
**
** DESCRIPTION:
**   Checks if the given command name is a built-in command.
**
** PARAMETERS:
**   cmd - Command name to check
**
** RETURN VALUE:
**   Returns 1 if built-in, 0 otherwise
*/
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}
/*
** execute_single_command - Execute a single command
**
** DESCRIPTION:
**   Executes a single command, checking for built-ins first.
**
** PARAMETERS:
**   cmd - Command structure with arguments
**   env - Environment variables
**
** RETURN VALUE:
**   Returns exit status of the command
*/
int	exe_single_cmd(t_arena *arena, t_cmd *cmd, char **env)
{
	if (!cmd || !cmd->cmd_av || !cmd->cmd_av[0] || !arena)
		return (0);
	// Check if it's a built-in command
	if (is_builtin(cmd->cmd_av[0]))
		return (exe_builtin(cmd, env));
	// Execute as external command
	return (exe_external_cmd(arena, cmd, env));
}
/*
** execute_external_command - Execute external program
**
** DESCRIPTION:
**   Forks and executes external program using execve.
**
** PARAMETERS:
**   cmd - Command structure
**   env - Environment variables
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
	else if (pid == 0) // Child process
	{
		if (execve(executable_path, cmd->cmd_av, env) == -1)
		{
			perror("minishell: execve");
			exit(126);
		}
	}
	else // Parent process
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else
			return (1);
	}
	return (0); // Should never reach here
}
