#include "minishell.h"

int	builtin_set(t_shell *shell, t_cmd *cmd)//DELETEEEEEEEEEEEEE
{
	char	*name = NULL;
	char	*value = NULL;
	int i = 0;
	while (cmd->cmd_av[1][i] && (cmd->cmd_av[1][i] != '='))
		name = ar_add_char_to_str(shell->arena, name, cmd->cmd_av[1][i++]);
	if (cmd->cmd_av[1][i] == '=')
		i++;
	while (cmd->cmd_av[1][i])
		value = ar_add_char_to_str(shell->arena, value, cmd->cmd_av[1][i++]);
	if (!set_shell_env_value(shell, name, value))
		return (-1);
	return (0);
}

int	builtin_export(t_shell *shell, t_cmd *cmd)
{
	char	*name;
	char	*value;
	size_t	i;

	name = NULL;
	value = NULL;
	i = 1;
	//test="$" ok, test="$abc" ambiguous
	if (!cmd->cmd_av[1])
	{
		plain_export(shell);
		return (0);
	}
	while (cmd->cmd_av[i])
	{
		if (export_this_var(shell, cmd->cmd_av[i]) == -1)
			return (-1);
		i++;
	}
	return (0);
}

void	plain_export(t_shell *shell)
{//only export -> print all exported variables
	while (shell->vars)
	{
		ft_printf("declare -x %s=\"%s\"", shell->vars->name,
			shell->vars->value);
		shell->vars = shell->vars->next_var;
	}
}

int	export_this_var(t_shell *shell, char *arg)
{
	t_var	*var;
	var = ar_alloc(shell->arena, sizeof(t_var));
	if (!var)
		return (err_msg_n_return_value("Allocation failed for a t_var struct\n",
			-1));
	//export w/o "=" -> do not register to env
	//export with "=" -> empty or smt in value. 
	// all quotes and var expansions apply to both name and value
	if (!ft_strchr(arg, "="))
	{
		//register to t_var;
	}
}

