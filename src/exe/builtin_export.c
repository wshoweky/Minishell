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

// int	builtin_export(t_shell *shell, t_cmd *cmd)
// {
// 	char	*name;
// 	char	*value;
// 	size_t	i;

// 	name = NULL;
// 	value = NULL;
// 	i = 1;
// 	//test="$" ok, test="$abc" ambiguous
// 	if (!cmd->cmd_av[1])
// 	{
// 		plain_export(shell);
// 		return (0);
// 	}
// 	while (cmd->cmd_av[i])
// 	{
// 		if (export_this_var(shell, cmd->cmd_av[i]) == -1)
// 			return (-1);
// 		i++;
// 	}
// 	return (0);
// }

// void	plain_export(t_shell *shell)
// {//only export -> print all exported variables
// 	while (shell->vars)
// 	{
// 		ft_printf("declare -x %s=\"%s\"", shell->vars->name,
// 			shell->vars->value);
// 		shell->vars = shell->vars->next_var;
// 	}
// }

// int	export_this_var(t_shell *shell, char *arg)
// {
// 	t_var	*var;
// 	var = ar_alloc(shell->arena, sizeof(t_var));
// 	if (!var)
// 		return (err_msg_n_return_value("Allocation failed for a t_var struct\n",
// 			-1));
// 	// all quotes and var expansions apply to both name and value
// 	//register to shell->vars, do sorting then add in

	
// 	if (ft_strchr(arg, "="))
// 	{
// 		//if nothing after =, value = ""
// 		if (find_name_and_value(shell, arg, &var) == -1)
// 			return (-1);
// 		if (!set_shell_env_value(shell, var->name, var->value))
// 			return (err_msg_n_return_value("Fail to export var to shell env\n",
// 				-1));
// 	}
// 	return (0);
// }

// int	find_name_and_value(t_shell *shell, char *arg, t_var **var)
// {
// 	size_t	i;

// 	i = 0;
// 	while (arg[i] && arg[i] != '=')
// 	{
// 		(*var)->name = ar_add_char_to_str(shell->arena, (*var)->name, arg[i]);
// 		if (!(*var)->name)
// 			return (err_msg_n_return_value("Error in building var name to "
// 				"export \n", -1));
// 	}
// 	i++;
// 	if (arg[i] == '\0')
// 	{
// 		(*var)->value = ar_alloc(shell->arena, 1);
// 		if (!(*var)->value)
// 			return (err_msg_n_return_value("Error in building var value to "
// 				"export\n", -1));
// 		(*var)->value[0] = '\0';
// 	}
// 	else
// 	{
// 		while (arg[i])
// 		{
// 			(*var)->value = ar_add_char_to_str(shell->arena, (*var)->value,
// 				arg[i]);
// 			if (!(*var)->value)
// 				return (err_msg_n_return_value("Error in building var value to "
// 					"export\n", -1));
// 			i++;
// 		}
// 	}
// 	return (0);
// }
