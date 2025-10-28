#include "minishell.h"

/* Copy the env variables (other than variable "_=[value]") to the export list
(might have updated value from shell env)

Return: 0 on success, -1 on errors
*/
int	copy_vars_fr_env_to_export_list(t_shell *shell)
{
	int		i;
	char	*copy;

	i = 0;
	while (i < shell->env_count)
	{
		copy = ar_strdup(shell->arena, shell->env[i]);
		if (!copy)
			return (err_msg_n_return_value("Copying env failed\n", -1));
		if (export_this_var(shell, copy) == -1)
			return (-1);
		i++;
	}
	return (0);
}

/*Entry point of the "export" function
- If no argument after export, print out all exported variables. Otherwise:
- Ensure there is no flag for export
- Work on each variable following "export" with helper function

Return: 0 on success, -1 on errors
*/
int	builtin_export(t_shell *shell, t_cmd *cmd)
{
	size_t	i;

	i = 1;
	if (copy_vars_fr_env_to_export_list(shell) == -1)
		return (-1);
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

/*Print out exported variables that were saved in the t_shell struct,
format based on if variable has a value assigned or not

Always return (0) for builtin_export() to return (0)
*/
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

/*Process the argument to export
- Parse name and value into t_var in memory arena
- Skip if name is "_"
- Copy data to heap t_var for persistence between commands
- Add to shell's exported variables list
- If variable has a value assigned, update shell environment

Return: 0 on success, -1 on errors
*/
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
	if (!ft_strcmp(arena_var->name, "_"))
		return (0);
	shell_var = ft_calloc(1, sizeof(t_var));
	if (!shell_var)
		return (err_msg_n_return_value("Calloc failed for t_var struct\n", -1));
	if (copy_var_fr_arena_to_shell(arena_var, shell_var) == -1)
		return (-1);
	register_to_shell_vars(shell, shell_var);
	if (ft_strchr(arg, '='))
	{
		if (!set_shell_env_value(shell, shell_var->name, shell_var->value))
			return (err_msg_n_return_value("Fail to export var to shell env\n",
					-1));
	}
	return (0);
}

/*Duplicating the data from the arena variable to the heap allocated variable
(helper function of export_this_var())

Return: 0 on success, -1 on errors
*/
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
