/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_cmd_table_tok.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 21:49:19 by gita              #+#    #+#             */
/*   Updated: 2025/10/05 13:06:28 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Process tokens for building command table
- Pipes: validate syntax then create a new t_cmd struct
- Redirections: validate file name, then create redirection expansion
- Words: pass to helper function for further processing

Return: 0 on success, -1 on errors
*/
int	check_current_token(t_arena *arena, t_tokens *token, t_cmd **current_cmd,
	t_cmd_table *table)
{
	if (token->type == TOKEN_PIPE)
	{
		if (!((*current_cmd)->cmd_av || (*current_cmd)->redirections)
			|| !token->next)
			return (err_msg_n_return_value("Syntax error around pipe\n", -1));
		(*current_cmd)->next_cmd = new_cmd_alloc(arena);
		if (!(*current_cmd)->next_cmd)
			return (err_msg_n_return_value("Memory allocation failed for "
					"new command\n", -1));
		*current_cmd = (*current_cmd)->next_cmd;
		table->cmd_count++;
		return (0);
	}
	else if (is_redirection(token->type))
	{
		if (!token->next || token->next->type != TOKEN_WORD)
			return (err_msg_n_return_value("Syntax error around redirection\n",
					-1));
		if (make_redir(arena, token, *current_cmd) == -1)
			return (-1);
		return (0);
	}
	else
		return (check_token_word(arena, token, *current_cmd));
}

/* Process word token and build the current command
- Validate if it is a word token
- Check if the string is just "&" or "&&" (these are not supported)
- If there is $ present, pass to helper function to expand variable name
- Add the value (can correctly be NULL) to the current command arguments

Return: 0 on success, -1 on errors
*/
int	check_token_word(t_arena *arena, t_tokens *token, t_cmd *current_cmd)
{
	if (token->type != TOKEN_WORD)
		return (err_msg_n_return_value("Not a word token\n", -1));
	if (ft_strcmp(token->value, "&") == 0 || ft_strcmp(token->value, "&&") == 0)
		return (err_msg_n_return_value("& and && not supported\n", -1));
	if (ft_strchr(token->value, '$'))
		if (expand_variable_name(arena, token) == -1)
			return (-1);
	if (add_argv(arena, current_cmd, token->value) == -1)
		return (-1);
	return (0);
}

/* Remove quotes if present and expand variable name
- Iterate through the token value to check each character
- Enter single/double quote mode and exit only when seeing the same quote
- Variable expansion according to the mode (in single quote or other modes)
- Other characters are added to the final string

Return: 0 on success, -1 on errors
*/
int	expand_variable_name(t_arena *arena, t_tokens *word_tok)
{
	//size_t	i;
	char	*expanded_text;

	//i = 0;	unused
	expanded_text = NULL;
	if (go_thru_input(arena, word_tok->value, &expanded_text) == -1)
		return (-1);
	word_tok->value = expanded_text;
	return (0);
}

/* Expand command array to include a new token value:
 - Allocating new array with space for existing arguments + new argument + NULL
 - Give existing string pointers to new array
 - Use ar_strdup() to duplicate the new token value into the array
 - Add NULL terminator to the expanded array
 - Update the old command array with the new allocation

 Return: 0 on success, -1 on errors
*/
int	add_argv(t_arena *arena, t_cmd *command, char *expansion)
{
	size_t	quantity;
	size_t	i;
	char	**new_cmd_av;

	quantity = 0;
	if (command->cmd_av)
		while (command->cmd_av[quantity])
			quantity++;
	new_cmd_av = ar_alloc(arena, (quantity + 2) * sizeof(char *));
	if (!new_cmd_av)
		return (err_msg_n_return_value("Allocation for command failed\n", -1));
	i = 0;
	if (command->cmd_av)
		get_old_argv(command->cmd_av, new_cmd_av, &i);
	new_cmd_av[i] = ar_strdup(arena, expansion);
	if (!new_cmd_av[i])
		return (err_msg_n_return_value("strdup fail while building command\n",
				-1));
	new_cmd_av[i + 1] = NULL;
	command->cmd_av = new_cmd_av;
	return (0);
}

/* Give the new cmd_av the same arguments the old cmd_av have
(helper function of add_argv())
*/
void	get_old_argv(char **old, char **new, size_t *i)
{
	while (old[*i])
	{
		new[*i] = old[*i];
		(*i)++;
	}
}
