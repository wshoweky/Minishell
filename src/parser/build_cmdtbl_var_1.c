#include "minishell.h"

/* Iterate through the input string and rewrite the input into expand_text
- If the character is $ and not in single quote mode, enter expansion function
- Otherwise, enter generic function for other cases

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
		if (input[i] == '$' && in_quote != 1)
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
- If land on a quote, switch on quote mode if currently not in one, or exit
quote mode only if it's the same quote
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
