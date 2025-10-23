#include "minishell.h"

/*Save the variable to the export list in the shell struct according to
alphabetical order. Update if there is duplicate variable name with a value
(helper function of export_this_var())

Return values (0 or 1) are not checked in calling function
*/
int	register_to_shell_vars(t_shell *shell, t_var *var)
{
	t_var	*find_place;
	t_var	*behind_this;

	if (!shell->vars)
		shell->vars = var;
	else
	{
		if (ft_strcmp(var->name, shell->vars->name) <= 0)
			return (destined_to_be_first(shell, var));
		else
		{
			behind_this = shell->vars;
			find_place = shell->vars->next_var;
			while (find_place && ft_strcmp(find_place->name, var->name) < 0)
			{
				behind_this = find_place;
				find_place = find_place->next_var;
			}
			if (find_place && ft_strcmp(find_place->name, var->name) == 0)
				return (update_this_fella(var, find_place, behind_this));
			var->next_var = find_place;
			behind_this->next_var = var;
		}
	}
	return (0);
}

/*Make variable the 1st node of the list. If it is a duplicate name, check if
the new input has a value assigned in order to update the node.
(helper function of register_to_shell_vars())

Return: 0 in normal cases, 1 when name is a duplicate and no value to update
*/
int	destined_to_be_first(t_shell *shell, t_var *var)
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
	else
	{
		var->next_var = shell->vars;
		shell->vars = var;
		return (0);
	}
}

/*Upon seeing a duplicate variable name, update that shell variable node if
there is a new value assigned.
(helper function of register_to_shell_vars())

Return: 0 in normal cases, 1 when name is a duplicate and no value to update
*/
int	update_this_fella(t_var *var, t_var *find, t_var *behind)
{
	if (!var->equal_sign)
	{
		free_1_var(&var);
		var = find;
		return (1);
	}
	else
	{
		behind->next_var = var;
		var->next_var = find->next_var;
		free_1_var(&find);
		return (0);
	}
}
