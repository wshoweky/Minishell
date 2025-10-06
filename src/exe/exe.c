/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 17:48:17 by wshoweky          #+#    #+#             */
/*   Updated: 2025/10/06 12:38:38 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	dispatch_builtin(t_cmd *cmd, char **env);

/*
** exe_cmd - Main command execution dispatcher
**
** DESCRIPTION:
**   Decides whether to execute as built-in or external command.
**   This is the main entry point for command execution.
**
** PARAMETERS:
**   arena     - Memory arena
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
	if (cmd_table->cmd_count == 1)
		exit_status = exe_single_cmd(arena, current_cmd, env);
	else
	{
		//	exit_status = execute_pipeline(arena, cmd_table, env);
		ft_printf("minishell: pipelines not yet implemented\n");
		exit_status = 1;
	}	
	return (exit_status);
}
/*
** exe_single_cmd - Execute a single command
**
** DESCRIPTION:
**   Executes a single command, checking for built-ins first.
**
** PARAMETERS:
**   arena - Memory arena
**   cmd   - Command structure with arguments
**   env   - Environment variables
**
** RETURN VALUE:
**   Returns exit status of the command
*/
int	exe_single_cmd(t_arena *arena, t_cmd *cmd, char **env)
{
	int		status;
	pid_t	pid;
	
	if (!cmd || !arena)
		return (0);	
	// Handle case where there are only redirections (no command to execute)
	if (!cmd->cmd_av || !cmd->cmd_av[0])
	{
		if (cmd->redirections)
		{
			// Process redirections in a child process since they might fail
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
				exit(0);  // Success - redirections processed
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
		return (0);  // No command and no redirections
	}	
	if (is_builtin(cmd->cmd_av[0]))
		return (dispatch_builtin(cmd, env));
	return (exe_external_cmd(arena, cmd, env));
}
/*
** exe_builtin - Execute built-in command
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
** dispatch_builtin - Dispatch builtin to correct execution context
**
** DESCRIPTION:
**   Decides whether to execute builtin in parent or child process.
**   Parent-only: cd, export, unset, exit (modify shell state).
**   Forkable: echo, pwd, env (safe to fork with redirections).
**
** PARAMETERS:
**   cmd - Command structure
**   env - Environment variables
**
** RETURN VALUE:
**   Returns exit status of built-in command
*/
int	dispatch_builtin(t_cmd *cmd, char **env)
{
	char	*cmd_name;

	if (!cmd || !cmd->cmd_av || !cmd->cmd_av[0])
		return (0);
	cmd_name = cmd->cmd_av[0];
	if (is_non_forkable_builtin(cmd_name))
	{
		// Non-forkable builtins execute in parent process
		// Redirections are ignored for these commands (like in bash)
		return (exe_builtin(cmd, env));
	}
	if (!cmd->redirections)
		return (exe_builtin(cmd, env));
	return (exe_builtin_with_fork(cmd, env));
}
