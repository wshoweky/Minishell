#include "minishell.h"

int	builtin_unset(t_shell *shell, t_cmd *cmd)
{
	size_t	i;

	if (!cmd->cmd_av[1])
		return (0);
	if (copy_vars_fr_env_to_export_list(shell) == -1)
		return (-1);
	i = 1;
	while (cmd->cmd_av[i])
	{
		if (ft_strcmp(cmd->cmd_av[i], "_") != 0)
			if (unset_this_var(shell, cmd->cmd_av[i]) == -1)
				return (-1);
		i++;
	}
	return (0);
}

int	unset_this_var(t_shell *shell, char *input)
{
	if (ft_strchr(input, '$') || ft_strchr(input, '&'))
		if (expand_variable_name(shell, &input, 0) == -1)
			return (-1);
	if (!unset_shell_env_value(shell, input))
		return (err_msg_n_return_value("Failed to unset var from shell env\n",
				-1));
	unset_from_shell_vars(shell, input);
	return (0);
}

void	unset_from_shell_vars(t_shell *shell, char *name)
{
	t_var	*before_match;
	t_var	*match;

	if (!shell->vars)
		return ;
	else
	{
		match = shell->vars;
		
		//find match (1st place and other places again?), connect, free node
	}
}
