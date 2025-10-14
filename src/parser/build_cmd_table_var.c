#include "minishell.h"

/* Iterate through the input string and rewrite the input into expand_text
- If the character is $ and not in single quote mode, enter expansion function
- Otherwise, enter generic funtion for other cases

Return: 0 on success, -1 on errors
*/
int	go_thru_input(t_shell *shell, char *input, char **expand_text)
{
	size_t	i;
	int		in_quote;

	i = 0;
	in_quote = 0;
	while (input[i])
	{
		if (input[i] == '$' && (in_quote != 1))
		{
			if (dollar_sign_encounter(shell, input, &i, expand_text) == -1)
				return (-1);
		}
		else
		{
			if (other_character(shell->arena, expand_text, input[i],
					&in_quote) == -1)
				return (-1);
		}
		i++;
	}
	return (0);
}

/*
- If land on a quote and not already in any quote mode, switch on corresponding
mode of quoting. If already in some quote mode, check if current character is
the same quote to exit that quote mode
- Otherwise, add the current character to the string under build

Return: 0 on success, -1 on errors
*/
int	other_character(t_arena *arena, char **expand_text, char current_char,
		int *in_quote)
{
	if ((current_char == '\"' || current_char == '\'') && !*in_quote)
	{
		if (current_char == '\"')
			*in_quote = 2;
		if (current_char == '\'')
			*in_quote = 1;
	}
	else if (current_char == '\"' && (*in_quote == 2))
		*in_quote = 0;
	else if (current_char == '\'' && (*in_quote == 1))
		*in_quote = 0;
	else
	{
		*expand_text = ar_add_char_to_str(arena, *expand_text, current_char);
		if (!*expand_text)
			return (err_msg_n_return_value("Error building string\n", -1));
	}
	return (0);
}

/* When encounter a $, extract the variable name that contains alphanumeric
or _ characters only, or only one ? after the $.
If there is a valid name,
	pass the variable name to helper function for expansion.
If not, add the lonely $ to the string in build.

Return: 0 on success, -1 on errors
*/
int	dollar_sign_encounter(t_shell *shell, char *input, size_t *i, char **text)
{
	char	*var_name;

	var_name = NULL;
	while (ft_isalnum(input[*i + 1]) || input[*i + 1] == '_' || input[*i
			+ 1] == '?')
	{
		var_name = ar_add_char_to_str(shell->arena, var_name, input[*i + 1]);
		if (!var_name)
			return (err_msg_n_return_value("Error building var name\n", -1));
		(*i)++;
		if (var_name[0] == '?' && input[*i + 1])
			break ;
	}
	if (var_name)
	{
		if (transform_var_name(shell, text, var_name) == -1)
			return (-1);
	}
	else
	{
		*text = ar_add_char_to_str(shell->arena, *text, '$');
		if (!*text)
			return (err_msg_n_return_value("Error adding $ to string\n", -1));
	}
	return (0);
}

/* Find the value of the variable name passed to the function,
(or NULL if there is no such variable name in the system)
then add that value to the string in building

Return: 0 on success, -1 on errors
*/
int	transform_var_name(t_shell *shell, char **text, char *var_name)
{
	char	*var_value;

	if (var_name[0] == '?')
	{
		if (var_name[1])
			return (err_msg_n_return_value("Not supposed to have longer "
					"name for $?\n", -1));
		var_value = ar_itoa(shell->arena, shell->last_exit_status);
	}
	else
		var_value = get_shell_env_value(shell, var_name);
	*text = ar_strjoin(shell->arena, *text, var_value);
	if (!*text)
		return (err_msg_n_return_value("Error in joining variable to string\n",
				-1));
	return (0);
}
