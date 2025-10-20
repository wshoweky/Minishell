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

static void	DELETE_all_var_print(t_shell *shell)
{
	if (shell->vars)
	{
		t_var *this_dude = shell->vars;
		int	i = 1;
		while (this_dude)
		{
			ft_printf("[AVP]Var #%i: name=%s, value=%s\n", i, this_dude->name, this_dude->value);
			i++;
			this_dude = this_dude->next_var;
		}
	}
}

int	builtin_export(t_shell *shell, t_cmd *cmd)
{
	char	*name;
	char	*value;
	size_t	i;

	name = NULL;
	value = NULL;
	i = 1;
	DELETE_all_var_print(shell);
	// printf("DEBUG: shell->vars address: %p\n", (void*)shell->vars);
	if (!cmd->cmd_av[1])
	{
		plain_export(shell);
		return (0);
	}
	while (cmd->cmd_av[i])
	{
		if (cmd->cmd_av[i][0] == '-')
			return (err_msg_n_return_value("No export option supported\n", -1));
		i++;
	}
	i = 1;
	while (cmd->cmd_av[i])
	{
		ft_printf("arg=%s\n", cmd->cmd_av[i]);
		if (export_this_var(shell, cmd->cmd_av[i]) == -1)
			return (-1);
		// printf("DEBUG: After export_this_var - shell->vars: %p\n", (void*)shell->vars);
		// DELETE_all_var_print(&shell);
		i++;
	}
	return (0);
}

void	plain_export(t_shell *shell)
{//only export -> print all exported variables
	t_var	*print_this;

	print_this = shell->vars;
	while (print_this)
	{
		if (print_this->equal_sign == 1)
			ft_printf("declare -x %s=\"%s\"\n", print_this->name,
				print_this->value);
		else if (print_this->equal_sign == 0)
			ft_printf("declare -x %s\n", print_this->name);
		print_this = print_this->next_var;
	}
}

int	export_this_var(t_shell *shell, char *arg)
{
	t_var	*arena_var;
	t_var	*shell_var;

	arena_var = ar_alloc(shell->arena, sizeof(t_var));
	if (!arena_var)
		return (err_msg_n_return_value("Allocation failed for a t_var struct\n",
			-1));
	if (find_name_and_value(shell, arg, &arena_var) == -1)
		return (-1);
	shell_var = ft_calloc(1, sizeof(t_var));
	if (!shell_var)
		return (err_msg_n_return_value("Calloc failed for a t_var struct\n", -1));
	if (copy_var_fr_arena_to_shell(arena_var, shell_var) == -1)
		return (-1);
	if (register_to_shell_vars(shell, shell_var) == -1)
		return (-1);
	if (ft_strchr(arg, '='))
	{
		if (!set_shell_env_value(shell, shell_var->name, shell_var->value))
			return (err_msg_n_return_value("Fail to export var to shell env\n",
				-1));
		shell_var->push_to_env = 1;
	}
	return (0);
}

int	find_name_and_value(t_shell *shell, char *arg, t_var **var)
{
	size_t	i;

	i = 0;
	while (arg[i] && arg[i] != '=')
	{
		(*var)->name = ar_add_char_to_str(shell->arena, (*var)->name, arg[i]);
		if (!(*var)->name)
			return (err_msg_n_return_value("Error allocating memory for var "
				"name in export\n", -1));
		i++;
	}
	if (ft_strchr((*var)->name, '$') || ft_strchr((*var)->name, '&'))
		if (expand_variable_name(shell, &(*var)->name, 0) == -1)
			return (-1);
	if ((*var)->name[0] == 0)
		return(err_msg_n_return_value("Not a valid identifier\n", -1));
// ft_printf("name = %s\n", (*var)->name);
	if (arg[i] == '=')
	{
		(*var)->equal_sign = 1;
		i++;
	}
	if (arg[i] == '\0')
	{
		(*var)->value = ar_alloc(shell->arena, 1);
		if (!(*var)->value)
			return (err_msg_n_return_value("Error allocating memory for var "
				"value in export\n", -1));
	}
	else
	{
		while (arg[i])
		{
			(*var)->value = ar_add_char_to_str(shell->arena, (*var)->value,
				arg[i]);
			if (!(*var)->value)
				return (err_msg_n_return_value("Error in building var value to "
					"export\n", -1));
			i++;
		}
		if (ft_strchr((*var)->value, '$') || ft_strchr((*var)->value, '&'))
			if (expand_variable_name(shell, &(*var)->value, 0) == -1)
				return (-1);
	}
	return (0);
}

int	copy_var_fr_arena_to_shell(t_var *arena_var, t_var *shell_var)
{
	shell_var->name = ft_strdup(arena_var->name);
	if (!shell_var->name)
		return (err_msg_n_return_value("Copying var name failed\n", -1));
	shell_var->value = ft_strdup(arena_var->value);
	if (!shell_var->value)
		return (err_msg_n_return_value("Copying var value failed\n", -1));
	shell_var->equal_sign = arena_var->equal_sign;
	return (0);
}

int	register_to_shell_vars(t_shell *shell, t_var *var)
{
	t_var	*find_place;
	t_var	*behind_this;

	if (!shell->vars)
		shell->vars = var; 
	else
	{
		if (ft_strcmp(var->name, shell->vars->name) <= 0)
		{
			if (ft_strcmp(var->name, shell->vars->name) == 0)
			{
				shell->vars->value = ft_strdup(var->value);
				if (!shell->vars->value)
					return (err_msg_n_return_value("Failed to update "
						"1st variable value\n", -1));
				free_vars(var);
				return (0);
			}
			var->next_var = shell->vars;
			shell->vars = var;
			return (0);
		}
		behind_this = shell->vars;
		find_place = shell->vars->next_var;
		while (find_place && ft_strcmp(find_place->name, var->name) < 0)
		{
			behind_this = find_place;
			find_place = find_place->next_var;
		}
		if (find_place && ft_strcmp(find_place->name, var->name) == 0)
		{
			if (ft_strcmp(var->name, "_"))
			{
				find_place->value = ft_strdup(var->value);
				if (!find_place->value)
					return (err_msg_n_return_value("Failed to update "
						"variable value\n", -1));
				free_vars(var);
				return (0);
			}
		}
		var->next_var = find_place;
		behind_this->next_var = var;
	}
	return (0);
}

