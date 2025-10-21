#include "minishell.h"

/*
** tokenize_input - Enhanced tokenizer for shell input parsing
**
** DESCRIPTION:
**   Performs lexical analysis on shell input, breaking it into tokens.
**   Handles operators (|, <, >, >>, <<), quoted strings, and words.
**   Skips whitespace and provides proper error handling.
**
** PARAMETERS:
**   arena - Memory arena for allocations
**   input - The command line string to tokenize
**
** RETURN VALUE:
**   Returns a linked list of tokens (t_tokens *) or NULL on error
*/
t_tokens	*tokenize_input(t_arena *arena, char *input)
{
	t_tokens	*head;
	int			i;

	if (!input || !*input)
		return (NULL);
	head = NULL;
	i = 0;
	while (input[i])
	{
		skip_whitespace(input, &i);
		if (!input[i])
			break ;
		if (!process_single_token(arena, input, &i, &head))
			return (NULL);
	}
	return (head);
}

void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
}

/*
** process_single_token - Process one token and add it to the list
**
** DESCRIPTION:
**   Extracts a single token from input, creates a token node,
**   and adds it to the token list. Handles error cleanup.
**
** PARAMETERS:
**   arena - Memory arena for allocations
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**   head  - Pointer to token list head (modified by reference)
**
** RETURN VALUE:
**   Returns head of token list or NULL on error
*/
t_tokens	*process_single_token(t_arena *arena, char *input, int *i,
			t_tokens **head)
{
	t_tokens	*new_token;
	char		*token_value;
	char		*pre_process_value;
	int			start_i;

	start_i = *i;
	new_token = NULL;
	token_value = extract_next_token(arena, input, i);
	if (!token_value) // Error handling
		return (NULL);
	new_token = create_token(arena, token_value);
	if (!new_token) // Error handling
		return (NULL);
	pre_process_value = ft_substr(input, start_i, *i - start_i);
	if (pre_process_value)
	{
		new_token->was_quoted = has_quotes(pre_process_value);
		free(pre_process_value);
	}
	add_to_end(head, new_token);
	// No need to free token_value as it's in the arena
	return (*head);
}

/*
** extract_next_token - Main token extraction dispatcher
**
** DESCRIPTION:
**   Determines the type of the next token and calls the appropriate
**   extraction function. This is the main logic hub for tokenization.
**
** PARAMETERS:
**   arena     - Memory arena for allocations
**   input     - The input string
**   i         - Pointer to current index (modified by reference)
**
** RETURN VALUE:
**   Returns allocated string containing the token value or NULL on error
*/
char	*extract_next_token(t_arena *arena, char *input, int *i)
{
	char	*string;

	string = NULL;
	if (chop_up_input(arena, input, i, &string) == -1)
		return (NULL);
	return (check_for_quoted_string(arena, string));
}

/*
** Check if the string has any quotes
** Returns 1 if yes, 0 if no
*/
int		has_quotes(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (1);
		i++;
	}
	return (0);
}
