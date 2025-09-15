#include "minishell.h"

t_tokens	*tokenize_input(char *input);
t_tokens	*process_single_token(char *input, int *i, t_tokens **head);
char		*extract_next_token(char *input, int *i, t_tokens **new_token);
char		*extract_quoted_token(char *input, int *i, t_tokens **new_token);
/*
** tokenize_input - Enhanced tokenizer for shell input parsing
**
** DESCRIPTION:
**   Performs lexical analysis on shell input, breaking it into tokens.
**   Handles operators (|, <, >, >>, <<), quoted strings, and words.
**   Skips whitespace and provides proper error handling.
**
** PARAMETERS:
**   input - The command line string to tokenize
**
** RETURN VALUE:
**   Returns a linked list of tokens (t_tokens *) or NULL on error
*/
t_tokens	*tokenize_input(char *input)
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
		if (!process_single_token(input, &i, &head))
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
**   input - The input string
**   i     - Pointer to current index (modified by reference)
**   head  - Pointer to token list head (modified by reference)
**
** RETURN VALUE:
**   Returns head of token list or NULL on error
*/
t_tokens	*process_single_token(char *input, int *i, t_tokens **head)
{
	t_tokens	*new_token;
	char		*token_value;

	new_token = NULL;
	token_value = extract_next_token(input, i, &new_token);
	if (!token_value) // Error handling
	{
		free_list_nodes(*head);
		return (NULL);
	}
	if (!new_token)	// Only create new node if not already created (quoted case)
	    new_token = new_node(token_value);
	if (!new_token)	// Error handling
	{
		free(token_value);
		free_list_nodes(*head);
		return (NULL);
	}
	add_to_end(head, new_token);
	free(token_value);
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
**   input     - The input string
**   i         - Pointer to current index (modified by reference)
**   new_token - Pointer to token pointer (for quoted strings)
**
** RETURN VALUE:
**   Returns allocated string containing the token value or NULL on error
*/
char	*extract_next_token(char *input, int *i, t_tokens **new_token)
{
	char	current_char;

	current_char = input[*i];
	*new_token = NULL;
	if (current_char == '|' || current_char == '<' || current_char == '>')
		return (extract_special_token(input, i));
	else if (current_char == '"' || current_char == '\'') // Escape the single quote \'#39 "#34 ASCII
		return (extract_quoted_token(input, i, new_token));
	else
		return (extract_word_token(input, i));
}
/*
** extract_quoted_token - Extract quoted string tokens
**
** DESCRIPTION:
**   Handles extraction of quoted strings (single or double quotes).
**   Preserves spaces within quotes and sets the was_quoted flag.
**   Returns NULL if quotes are unclosed (syntax error).
**
** PARAMETERS:
**   input     - The input string
**   i         - Pointer to current index (modified by reference)
**   new_token - Pointer to token pointer (sets was_quoted flag)
**
** RETURN VALUE:
**   Returns allocated string with quote contents or NULL on error
*/
char	*extract_quoted_token(char *input, int *i, t_tokens **new_token)
{
	char	quote;
	int		start;
	char	*token_value;

	quote = input[*i]; // Remember which quote type (" or ')
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (input[*i] != quote) // Error: no matching quote
		return (NULL);
	// Extract content between quotes
	token_value = ft_substr(input, start, *i - start);
	(*i)++; // Skip closing quote
	*new_token = new_node(token_value);
	if (*new_token)
	{
		(*new_token)->type = TOKEN_WORD;
		(*new_token)->was_quoted = 1;
	}
	return (token_value);
}
