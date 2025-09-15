#include "minishell.h"

t_token_type	get_token_type(char *str)
{
	if (!str || !*str)
		return (TOKEN_EOF);
	if (ft_strcmp(str, "|") == 0)
		return (TOKEN_PIPE);
	if (ft_strcmp(str, "<") == 0)
		return (TOKEN_REDIRECT_IN);
	if (ft_strcmp(str, ">") == 0)
		return (TOKEN_REDIRECT_OUT);
	if (ft_strcmp(str, ">>") == 0)
		return (TOKEN_APPEND);
	if (ft_strcmp(str, "<<") == 0)
		return (TOKEN_HEREDOC);
	return (TOKEN_WORD);
}
// to be printed for debugging
char	*get_token_type_name(t_token_type type)
{
	if (type == TOKEN_WORD)
		return ("WORD");
	if (type == TOKEN_PIPE)
		return ("PIPE");
	if (type == TOKEN_REDIRECT_IN)
		return ("REDIRECT_IN");
	if (type == TOKEN_REDIRECT_OUT)
		return ("REDIRECT_OUT");
	if (type == TOKEN_APPEND)
		return ("APPEND");
	if (type == TOKEN_HEREDOC)
		return ("HEREDOC");
	return ("EOF");
}

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == ' ' || c == '\t' 
		|| c == '\n' || c == '"' || c == '\'');
}

/*
** skip_whitespace - Skip whitespace characters in input string
**
** DESCRIPTION:
**   Advances the index pointer past any whitespace characters (space, tab).
**   This helper function ensures tokens are properly separated.
**
** PARAMETERS:
**   input - The input string to process
**   i     - Pointer to current index (modified by reference)
*/
void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
}