#include "minishell.h"

static int	get_home_path(t_shell *shell, char **path)
{
	char	*home;

	home = get_shell_env_value(shell, "HOME");
	if (!home)
		return (ft_printf("cd: HOME not set\n"), 1);
	*path = home;
	return (0);
}

static int	get_oldpwd_path(t_shell *shell, char **path)
{
	char	*oldpwd;

	oldpwd = get_shell_env_value(shell, "OLDPWD");
	if (!oldpwd)
		return (ft_printf("cd: OLDPWD not set\n"), 1);
	*path = oldpwd;
	ft_printf("%s\n", *path);
	return (0);
}

static int	resolve_cd_path(t_shell *shell, t_cmd *cmd, char **path)
{
	if (!cmd->cmd_av[1])
		return (get_home_path(shell, path));
	else if (ft_strcmp(cmd->cmd_av[1], "-") == 0)
		return (get_oldpwd_path(shell, path));
	else
	{
		if (cmd->cmd_av[2])
			return (ft_printf("cd: too many arguments\n"), 1);
		*path = cmd->cmd_av[1];
	}
	return (0);
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*path;

	if (!shell || !cmd || !cmd->cmd_av)
		return (1);
	if (!shell->cwd)
		return (ft_printf("cd: current directory not set\n"), 1);
	if (resolve_cd_path(shell, cmd, &path) != 0)
		return (1);
	if (chdir(path) != 0)
		return (perror("cd"), 1);
	if (!update_shell_cwd(shell, shell->cwd))
		return (ft_printf("cd: failed to update environment\n"), 1);
	return (0);
}
