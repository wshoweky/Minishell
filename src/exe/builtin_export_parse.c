#include "minishell.h"

/* Parse a single argument and save data to the arena t_var
(helper function of export_this_var())

Return: 0 on success, -1 on errors
*/
int	find_name_and_value(t_shell *shell, char *arg, t_var **var)
{
	size_t	i;

	i = 0;
	if (this_is_name(shell, arg, &i, var) == -1)
		return (-1);
	if (arg[i] == '=')
	{
		(*var)->equal_sign = 1;
		i++;
	}
	if (this_is_value(shell, arg, &i, var) == -1)
		return (-1);
	return (0);
}

/*Extract the name part of the variable, perform expansion if needed.
Prevent further execution if name is not valid or empty
(helper function of find_name_and_value())

Return: 0 on success, -1 on errors
*/
int	this_is_name(t_shell *shell, char *arg, size_t *i, t_var **var)
{
	while (arg[*i] && arg[*i] != '=')
	{
		if (!(ft_isalnum(arg[*i]) || arg[*i] == '_'))
			return (err_msg_n_return_value("Not a valid identifier\n", -1));
		(*var)->name = ar_add_char_to_str(shell->arena, (*var)->name, arg[*i]);
		if (!(*var)->name)
			return (err_msg_n_return_value("Error allocating memory for var "
					"name in export\n", -1));
		(*i)++;
	}
	if (!(*var)->name)
		return (err_msg_n_return_value("Not a valid identifier\n", -1));
	if (ft_strchr((*var)->name, '$') || ft_strchr((*var)->name, '&'))
		if (expand_variable_name(shell, &(*var)->name, 0) == -1)
			return (-1);
	if ((*var)->name[0] == 0 || ft_isdigit((*var)->name[0])
		|| !ft_strcmp((*var)->name, "&") || !ft_strcmp((*var)->name, "&&"))
		return (err_msg_n_return_value("Not a valid identifier\n", -1));
	return (0);
}

/*Extract the value part of the variable, perform expansion if needed.
If there is no value assigned, variable value will be an empty string
(helper function of find_name_and_value())

Return: 0 on success, -1 on errors
*/
int	this_is_value(t_shell *shell, char *arg, size_t *i, t_var **var)
{
	if (arg[*i] == '\0')
	{
		(*var)->value = ar_alloc(shell->arena, 1);
		if (!(*var)->value)
			return (err_msg_n_return_value("Error allocating memory for var "
					"value in export\n", -1));
	}
	else
	{
		while (arg[*i])
		{
			(*var)->value = ar_add_char_to_str(shell->arena, (*var)->value,
					arg[*i]);
			if (!(*var)->value)
				return (err_msg_n_return_value("Error in building var value to "
						"export\n", -1));
			(*i)++;
		}
		if (ft_strchr((*var)->value, '$') || ft_strchr((*var)->value, '&'))
			if (expand_variable_name(shell, &(*var)->value, 0) == -1)
				return (-1);
	}
	return (0);
}
