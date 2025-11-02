#include "minishell.h"

/*Entry point of the "unset" function
- Copy shell env variables to export list
- If no argument after unset, do nothing. Otherwise:
- Ensure there is no flag for unset
- Work on each variable following "unset" with helper function

Return: 0 on success, 1 on errors
*/
int	builtin_unset(t_shell *shell, t_cmd *cmd)
{
	size_t	i;

	if (!cmd->cmd_av[1])
		return (0);
	if (copy_vars_fr_env_to_export_list(shell) == -1)
		return (1);
	i = 1;
	while (cmd->cmd_av[i])
	{
		if (cmd->cmd_av[i][0] == '-')
			return (err_msg_n_return_value("No export option supported\n", 1));
		i++;
	}
	i = 1;
	while (cmd->cmd_av[i])
	{
		if (ft_strcmp(cmd->cmd_av[i], "_") != 0)
			if (unset_this_var(shell, cmd->cmd_av[i]) == -1)
				return (1);
		i++;
	}
	return (0);
}

/*Unset each argument in both shell env and export lists if, after
doing expansion if needed, variable name matches the existing variable name
(helper function of builtin_unset())

Return: 0 on success, -1 on errors
*/
int	unset_this_var(t_shell *shell, char *input)
{
	if (ft_strchr(input, '$') || ft_strchr(input, '&'))
		if (expand_variable_name(shell, &input, 0) == -1)
			return (-1);
	if (ft_isdigit(input[0]) || !ft_strcmp(input, "&") || !ft_strcmp(input,
			"&&"))
		return (err_msg_n_return_value("Not a valid identifier\n", -1));
	if (unset_shell_env_value(shell, input) == -1)
		return (err_msg_n_return_value("Failed to unset var from shell env\n",
				-1));
	unset_from_shell_vars(shell, input);
	return (0);
}

/*Find the matching variable name in the exportted list and get rid of it
(helper function of unset_this_var())
*/
void	unset_from_shell_vars(t_shell *shell, char *name)
{
	t_var	*match_at_1st_sight;

	if (!shell->vars)
		return ;
	else
	{
		if (!ft_strcmp(shell->vars->name, name))
		{
			match_at_1st_sight = shell->vars;
			shell->vars = shell->vars->next_var;
			free_1_var(&match_at_1st_sight);
		}
		else
		{
			perform_exorcism_on_doppelganger(shell, name);
		}
		return ;
	}
}

/*- It's a Halloween themed shell, don't question the function name
- Find the existing exported variable with the same name passed.
Delete the node if such variable is found
*/
void	perform_exorcism_on_doppelganger(t_shell *shell, char *name)
{
	t_var	*before_match;
	t_var	*match;

	before_match = shell->vars;
	match = shell->vars->next_var;
	while (match && ft_strcmp(match->name, name))
	{
		match = match->next_var;
		before_match = before_match->next_var;
	}
	if (match)
	{
		before_match->next_var = match->next_var;
		free_1_var(&match);
	}
}
