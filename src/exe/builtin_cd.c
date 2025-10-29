#include "minishell.h"

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*path;
	char	*home;
	char	*oldpwd;

	if (!shell || !cmd || !cmd->cmd_av)
		return (1);
	if (!shell->cwd)
		return (ft_printf("cd: current directory not set\n"), 1);
	if (!cmd->cmd_av[1]) // changing home ~
	{
		home = get_shell_env_value(shell, "HOME");
		if (!home)
			return (ft_printf("cd: HOME not set\n"), 1);
		path = home;
	}
	else if (ft_strcmp(cmd->cmd_av[1], "-") == 0) // changing back
	{
		oldpwd = get_shell_env_value(shell, "OLDPWD");
		if (!oldpwd)
			return (ft_printf("cd: OLDPWD not set\n"), 1);
		path = oldpwd;
		ft_printf("%s\n", path);
	}
	else
	{
		if (cmd->cmd_av[2]) // error check
			return (ft_printf("cd: too many arguments\n"), 1);
		path = cmd->cmd_av[1];
	}
	if (chdir(path) != 0)
		return (perror("cd"), 1);
	if (!update_shell_cwd(shell, shell->cwd))
		return (ft_printf("cd: failed to update environment\n"), 1);
	return (0);
}
