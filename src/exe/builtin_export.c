#include "minishell.h"

int	builtin_export(t_shell *shell, t_cmd *cmd)
{
	char	*name;
	char	*value;
	size_t	i;

	name = NULL;
	value = NULL;
	i = 1;
	if (!cmd->cmd_av[1])
		return (plain_export(shell));
	while (cmd->cmd_av[i])
	{
		if (cmd->cmd_av[i][0] == '-')
			return (err_msg_n_return_value("No export option supported\n", -1));
		i++;
	}
	i = 1;
	while (cmd->cmd_av[i])
	{
		if (export_this_var(shell, cmd->cmd_av[i]) == -1)
			return (-1);
		i++;
	}
	return (0);
}

int	plain_export(t_shell *shell)
{
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
	return (0);
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
				if (!var->equal_sign)
				{
					free_1_var(&var);
					var = shell->vars;
					return (1);
				}
				var->next_var = shell->vars->next_var;
				free_1_var(&shell->vars);
				shell->vars = var;
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
			if (ft_strcmp(var->name, "_") != 0)
			{
				if (!var->equal_sign)
				{
					free_1_var(&var);
					var = find_place;
					return (1);
				}
				behind_this->next_var = var;
				var->next_var = find_place->next_var;
				free_1_var(&find_place);
				return (0);
			}
		}
		var->next_var = find_place;
		behind_this->next_var = var;
	}
	return (0);
}

