/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:32:53 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/03 17:36:28 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	expand_tilde_path(t_shell *shell, char *arg, char **path)
{
	char	*home;
	char	*rest;

	home = get_shell_env_value(shell, "HOME");
	if (!home)
		return (print_error("cd", NULL, "HOME not set"), 1);
	if (arg[1] == '\0')
		*path = home;
	else if (arg[1] == '/')
	{
		rest = &arg[1];
		*path = ar_strjoin(shell->arena, home, rest);
		if (!*path)
			return (print_error("cd", NULL, "memory allocation failed"), 1);
	}
	else
		*path = arg;
	return (0);
}

static int	get_home_path(t_shell *shell, char **path)
{
	char	*home;

	home = get_shell_env_value(shell, "HOME");
	if (!home)
		return (print_error("cd", NULL, "HOME not set"), 1);
	*path = home;
	return (0);
}

static int	get_oldpwd_path(t_shell *shell, char **path)
{
	char	*oldpwd;

	oldpwd = get_shell_env_value(shell, "OLDPWD");
	if (!oldpwd)
		return (print_error("cd", NULL, "OLDPWD not set"), 1);
	*path = oldpwd;
	ft_printf("%s\n", *path);
	return (0);
}

static int	resolve_cd_path(t_shell *shell, t_cmd *cmd, char **path)
{
	if (!cmd->cmd_av[1])
		return (get_home_path(shell, path));
	if (ft_strcmp(cmd->cmd_av[1], "-") == 0)
		return (get_oldpwd_path(shell, path));
	if (cmd->cmd_av[2])
		return (print_error("cd", NULL, "too many arguments"), 1);
	if (cmd->cmd_av[1][0] == '~')
		return (expand_tilde_path(shell, cmd->cmd_av[1], path));
	*path = cmd->cmd_av[1];
	return (0);
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*path;

	if (!shell || !cmd || !cmd->cmd_av)
		return (1);
	if (!shell->cwd)
		return (print_error("cd", NULL, "current directory not set"), 1);
	if (resolve_cd_path(shell, cmd, &path) != 0)
		return (1);
	if (chdir(path) != 0)
		return (perror("cd"), 1);
	if (!update_shell_cwd(shell, shell->cwd))
		return (print_error("cd", NULL, "failed to update environment"), 1);
	return (0);
}
