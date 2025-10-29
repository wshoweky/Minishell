#include "minishell.h"

/**
** special_heredoc_delimiter - Remove most outer quotes (if any) from 
heredoc delimiter which contains $ or &
**
** BASH BEHAVIOR:
** - Quotes are removed from delimiter: "$USER" → $USER
** - But their presence disables variable expansion in content
**
** EXAMPLES:
**   "$USER" → $USER
**   '"$A"' → "$A"
**
**   delimiter - Original delimiter string with $ or & (may contain quotes)
**
**   Returns: Arena-allocated string with outer quotes removed or look the same
**	with original string that contains no quotes
*/
char	*special_heredoc_delimiter(t_arena *arena, char *delimiter)
{
	char	*result;

	if (!delimiter)
		return (NULL);
	result = ar_alloc(arena, ft_strlen(delimiter) + 1);
	if (!result)
		return (NULL);
	if (parse_special_delimiter(arena, delimiter, &result) == -1)
		return (NULL);
	return (result);
}

/* Other than most outer quotes (if present), add all characters from original
delimiter to the result string.
(helper function of special_heredoc_delimiter())

 Return: 0 on success, -1 on error
*/
int	parse_special_delimiter(t_arena *arena, char *delimiter, char **result)
{
	char	quote;
	int		in_quote;
	int		i;

	i = 0;
	in_quote = 0;
	while (delimiter[i])
	{
		if (!in_quote && (delimiter[i] == '"' || delimiter[i] == '\''))
		{
			in_quote = 1;
			quote = delimiter[i];
		}
		else if (in_quote && delimiter[i] == quote)
			in_quote = 0;
		else
		{
			*result = ar_add_char_to_str(arena, *result, delimiter[i]);
			if (!*result)
				return (err_msg_n_return_value("Failed while building "
					"heredoc eof\n", -1));
		}
		i++;
	}
	return (0);
}
