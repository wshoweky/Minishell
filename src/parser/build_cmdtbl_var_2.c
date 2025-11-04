/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_cmdtbl_var_2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 23:46:44 by gita              #+#    #+#             */
/*   Updated: 2025/11/03 23:46:45 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* When encounter a $, enter helper function to extract the variable name 
- If there is a valid variable name,
pass the variable name to helper function for expansion.
- If no variable name, depends on the situation to add the lonely $ 
to the string in build or not

Return: 0 on success, -1 on errors
*/
int	dollar_sign_encounter(t_shell *shell, char *input, size_t *i, char **text)
{
	char	*var_name;
	int		check;

	var_name = NULL;
	check = build_var_name(shell, input, i, &var_name);
	if (check == -1)
		return (-1);
	if (var_name)
	{
		if (transform_var_name(shell, text, var_name) == -1)
			return (-1);
	}
	else
	{
		if (check != 1)
		{
			*text = ar_add_char_to_str(shell->arena, *text, '$');
			if (!*text)
				return (err_msg_n_return_value("Error adding $ to str\n", -1));
		}
	}
	return (0);
}

/* Build the variable name that contains alphanumeric or _ characters,
or only one ? after the $. No building if a quote is found after $ and enter
helper function to decide to print out $ or not

Return: 1 when $ should NOT be printed, 0 when $ should be printed or
variable name was built successfully, -1 on errors
*/
int	build_var_name(t_shell *shell, char *input, size_t *i, char **var_name)
{
	int	print_sign;

	if (input[*i + 1] == '"' || input[*i + 1] == '\'')
	{
		print_sign = check_dollar_sign_position(input, i);
		if (print_sign == 0)
			return (1);
		if (print_sign == 1)
			return (0);
	}
	while (ft_isalnum(input[*i + 1]) || input[*i + 1] == '_'
		|| input[*i + 1] == '?')
	{
		*var_name = ar_add_char_to_str(shell->arena, *var_name, input[*i + 1]);
		if (!*var_name)
			return (err_msg_n_return_value("Error building var name\n", -1));
		(*i)++;
		if (*var_name[0] == '?' && input[*i + 1])
			return (0);
	}
	return (0);
}

/* 0 = do not print, 1 = print
*/
int	check_dollar_sign_position(char *input, size_t *i)
{
	size_t	p;
	int		in_quotes;
	char	quote;

	p = 0;
	in_quotes = 0;
	while (input[p] && p <= *i)
	{
		if (!in_quotes && (input[p] == '"' || input[p] == '\''))
		{
			in_quotes = 1;
			quote = input[p];
		}
		else if (in_quotes && input[p] == quote)
			in_quotes = 0;
		p++;
	}
	if (in_quotes)
		return (1);
	else
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
