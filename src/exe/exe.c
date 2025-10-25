#include "minishell.h"

int	dispatch_builtin(t_cmd *cmd, t_shell *shell);

/*
** exe_cmd - Main command execution dispatcher
**
** DESCRIPTION:
**   Routes execution to single command or pipeline based on command count.
**   Single commands execute directly, pipelines use pipe infrastructure.
**
** PARAMETERS:
**   shell     - Shell state structure
**   cmd_table - Command table from parser
**
** RETURN VALUE:
**   Returns exit status of executed command(s)
*/
int	exe_cmd(t_shell *shell, t_cmd_table *cmd_table)
{
	if (!cmd_table || !cmd_table->list_of_cmds || !shell)
		return (0);
	if (handle_heredocs(shell, cmd_table) != 0)
	{
		cleanup_heredoc_files(cmd_table);
		return (1);
	}
	if (cmd_table->cmd_count > 1)
	{
		execute_pipeline(shell, cmd_table);
		return (shell->last_exit_status);
	}
	return (exe_single_cmd(shell, cmd_table->list_of_cmds));
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
**   cmd   - Command structure
**   shell - Shell state structure
**
** RETURN VALUE:
**   Returns exit status of built-in command
*/
int	dispatch_builtin(t_cmd *cmd, t_shell *shell)
{
	char	*cmd_name;

	if (!cmd || !cmd->cmd_av || !cmd->cmd_av[0])
		return (0);
	cmd_name = cmd->cmd_av[0];
	if (is_non_forkable_builtin(cmd_name))
	{
		// Non-forkable builtins execute in parent process
		// Redirections are ignored for these commands (like in bash)
		return (exe_builtin(cmd, shell));
	}
	if (!cmd->redirections)
		return (exe_builtin(cmd, shell));
	return (exe_builtin_with_fork(cmd, shell));
}

/*
** exe_single_cmd - Execute a single command
**
** DESCRIPTION:
**   Executes a single command: builtin, external, or redirection-only.
**   Dispatches to appropriate handler based on command type.
**
** PARAMETERS:
**   shell - Shell state structure
**   cmd   - Command structure with arguments
**
** RETURN VALUE:
**   Returns exit status of the command
*/
int	exe_single_cmd(t_shell *shell, t_cmd *cmd)
{
	if (!cmd || !shell)
		return (0);
	if (!cmd->cmd_av || !cmd->cmd_av[0])
		return (exe_redirection_only(cmd));
	if (is_builtin(cmd->cmd_av[0]))
		return (dispatch_builtin(cmd, shell));
	return (exe_external_cmd(shell, cmd));
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
** exe_builtin - Execute built-in command
**
** DESCRIPTION:
**   Dispatches to appropriate built-in command handler.
**
** PARAMETERS:
**   cmd   - Command structure
**   shell - Shell state structure
**
** RETURN VALUE:
**   Returns exit status of built-in command
*/
int	exe_builtin(t_cmd *cmd, t_shell *shell)
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
		return (builtin_env(shell));
	else if (ft_strcmp(cmd_name, "exit") == 0)
		return (builtin_exit(cmd));
	else if (ft_strcmp(cmd_name, "export") == 0)
		return (builtin_export(shell, cmd));
	else if (ft_strcmp(cmd_name, "unset") == 0)
		return (builtin_unset(shell, cmd));
	ft_printf("Built-in '%s' not yet implemented\n", cmd_name);
	return (1);
}
