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
// 	t_var	*print_this;

// 	print_this = shell->vars;
// 	while (print_this)
// 	{
// 		if (print_this->equal_sign == 1)
// 			ft_printf("declare -x %s=\"%s\"\n", print_this->name,
// 				print_this->value);
// 		else if (print_this->equal_sign == 0)
// 			ft_printf("declare -x %s\n", print_this->name);
// 		print_this = print_this->next_var;
// 	}
// }

// int	export_this_var(t_shell *shell, char *arg)
// {
// 	t_var	*var;
// 	var = ar_alloc(shell->arena, sizeof(t_var));
// 	if (!var)
// 		return (err_msg_n_return_value("Allocation failed for a t_var struct\n",
// 			-1));
// 	if (find_name_and_value(shell, arg, &var) == -1)
// 		return (-1);
// 	if (register_to_shell_vars(shell, var) == -1)
// 		return (-1);
// 	if (ft_strchr(arg, "="))
// 	{
// 		if (!set_shell_env_value(shell, var->name, var->value))
// 			return (err_msg_n_return_value("Fail to export var to shell env\n",
// 				-1));
// 		var->push_to_env = 1;
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
// 			return (err_msg_n_return_value("Error allocating memory for var "
// 				"name in export\n", -1));
// 	}
// 	if (ft_strchr((*var)->name, '$') || ft_strchr((*var)->name, '&'))
// 		if (expand_variable_name(shell, (*var)->name, 0) == -1)
// 			return (-1);
// 	if ((*var)->name[0] == 0)
// 		return(err_msg_n_return_value("Not a valid identifier\n", -1));
// 	if (arg[i] == '=')
// 	{
// 		(*var)->equal_sign = 1;
// 		i++;
// 	}
// 	if (arg[i] == '\0')
// 	{
// 		(*var)->value = ar_alloc(shell->arena, 1);
// 		if (!(*var)->value)
// 			return (err_msg_n_return_value("Error allocating memory for var "
// 				"value in export\n", -1));
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
// 		if (ft_strchr((*var)->value, '$') || ft_strchr((*var)->value, '&'))
// 			if (expand_variable_name(shell, (*var)->value, 0) == -1)
// 				return (-1);
// 	}
// 	return (0);
// }

// int	register_to_shell_vars(t_shell *shell, t_var *var)
// {
// 	t_var	*find_place;

// 	if (!shell->vars)
// 		shell->vars = var;
// 	else
// 	{
// 		find_place = shell->vars;
// 		while (find_place && ft_strcmp(find_place->name, var->name) < 0)
// 		{
// 			find_place = find_place->next_var;
// 		}
// 		if (ft_strcmp(find_place->name, var->name) == 0)
// 		{
// 			if (ft_strcmp(var->name, "_"))
// 				find_place->value = var->value;
// 			return (0);
// 		}
// 		var->next_var = find_place->next_var;
// 		find_place->next_var = var;
// 	}
// 	return (0);
// }

