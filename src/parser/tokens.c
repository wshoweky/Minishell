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
t_tokens	*process_single_token(t_arena *arena, char *input, int *i, t_tokens **head)
{
	t_tokens	*new_token;
	char		*token_value;

	new_token = NULL;
	token_value = extract_next_token(arena, input, i);
	if (!token_value) // Error handling
		return (NULL);
    new_token = create_token(arena, token_value);
	if (!new_token)	// Error handling
		return (NULL);
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
	int	in_quotes;
	char quote;
	int	start_quote;
	char *string;

	in_quotes = 0;
	string = NULL;
	while (input[*i])
	{
		if (in_quotes)
		{
			string = ar_add_char_to_str(arena, string, input[*i]);
			if (input[*i] == quote)
				in_quotes = 0;
		}
		else
		{
			if (input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
			{
				if (string)
					break ;
				else
					return (extract_special_token(arena, input, i));
			}
			else if (input[*i] == '"' || input[*i] == '\'')
			{
				in_quotes = 1;
				quote = input[*i];
				start_quote = *i;
				string = ar_add_char_to_str(arena, string, input[*i]);
				if (!string)
					return (err_msg_n_return_null("Memory alloc fail for quote"));
			}
			else if (input[*i] == ' ' || input[*i] == '\t' || input[*i] == '\n')
				break ;
			else
				string = ar_add_char_to_str(arena, string, input[*i]);
		}
		(*i)++;
	}
	string = check_for_quoted_string(arena, string);
	return (string);
}

/* Check for quotes in the string
- Return NULL if there is unclosed quote
- If no $ is present in the string, remove quotes (that are outside of quotes)
and return a clean string, or NULL if errors
- If there is $, return the string as it is
*/
char	*check_for_quoted_string(t_arena *arena, char *str)
{
	size_t	i;
	char	*output;
	char	quote;
	int		in_quote;

	i = 0;
	output = NULL;
	in_quote = 0;
	quote = 0;

	while (str[i])
	{
		if (!in_quote && (str[i] == '"' || str[i] == '\''))
		{
			quote = str[i];
			in_quote = 1;
		}
		else if (in_quote && str[i] == quote)
			in_quote = 0;
		i++;
	}
	if (in_quote)
		return (err_msg_n_return_null("Unclosed quote\n"));
	if (!ft_strchr(str, '$'))
	{
		i = 0;
		while (str[i])
		{
			if (!in_quote && (str[i] == '"' || str[i] == '\''))
			{
				quote = str[i];
				in_quote = 1;
			}
			else if (in_quote && str[i] == quote)
				in_quote = 0;
			else
			{
				output = ar_add_char_to_str(arena, output, str[i]);
				if (!output)
					return (err_msg_n_return_null("Failed making string not quoted anymore\n"));
			}
			i++;
		}
		return (output);
	}
	else
		return (str);
}
