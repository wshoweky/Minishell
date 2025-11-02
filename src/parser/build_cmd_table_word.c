#include "minishell.h"

/* Process word token and build the current command
- Validate if it is a word token
- Check if the string is just "&" or "&&" (these are not supported)
- If there is $ present, pass to helper function to expand variable name
- Add the value (can correctly be NULL) to the current command arguments

Return: 0 on success, -1 on errors
*/
int	check_token_word(t_shell *shell, t_tokens *token, t_cmd *current_cmd)
{
	if (token->type != TOKEN_WORD)
		return (err_msg_n_return_value("Not a word token\n", -1));
	if (ft_strcmp(token->value, "&") == 0 || ft_strcmp(token->value, "&&") == 0)
		return (err_msg_n_return_value("& and && not supported\n", -1));
	if (ft_strchr(token->value, '$') || ft_strchr(token->value, '&'))
		if (expand_variable_name(shell, &token->value, 0) == -1)
			return (-1);
	if ((!token->value || !token->value[0]) && !token->was_quoted)
		return (0);
	if (add_argv(shell->arena, current_cmd, token->value) == -1)
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
int	expand_variable_name(t_shell *shell, char **original_string, int in_redir)
{
	char	*expanded_text;

	expanded_text = NULL;
	if (go_thru_input(shell, *original_string, &expanded_text) == -1)
		return (-1);
	if (in_redir)
	{
		if (expanded_text[0] == 0)
			return (err_msg_n_return_value("Ambiguous redirect\n", -1));
		if (var_in_redir_outside_2xquotes(*original_string) == 1)
			if (ft_strchr(expanded_text, ' '))
				return (err_msg_n_return_value("Ambiguous redirect\n", -1));
	}
	*original_string = expanded_text;
	return (0);
}


int	var_in_redir_outside_2xquotes(char *tok_value)
{
	int		i;
	int		in_quote;

	i = 0;
	in_quote = 0;
	while (tok_value[i])
	{
		if (tok_value[i] == '$')
		{
			if (!in_quote)
				return (1);
		}
		else
		{
			if (tok_value[i] == '"' && !in_quote)
				in_quote = 1;
			else if (tok_value[i] == '"' && in_quote)
				in_quote = 0;
		}
		i++;
	}
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
