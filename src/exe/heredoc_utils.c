#include "minishell.h"

static int	expand_dollar_sign(t_shell *shell, char *input, size_t *i,
		char **text)
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

/**
** expand_heredoc_line - Expand variables in heredoc line
**
** HEREDOC EXPANSION RULES (Bash-compatible):
** 1. Variables ALWAYS expand (controlled by delimiter quotes, not content)
** 2. ALL characters are preserved literally (including quotes)
** 3. Quotes in content are just regular characters, not special
**
** IMPORTANT: Expansion control is determined by the DELIMITER, not content:
**   << EOF   → expands (this function is called)
**   << 'EOF' → doesn't expand (this function is NOT called)
**
** EXAMPLES:
**   "$VAR"  → "value"   (quotes preserved, variable expanded)
**   '$VAR'  → 'value'   (quotes preserved, variable expanded)
**   text    → text      (no change)
**
** This is different from command-line parsing where quotes are removed.
**
**   shell - Shell state for variable expansion
**   line  - Original line with potential variables
**
**   Returns: Expanded string or NULL on error
*/
char	*expand_heredoc_line(t_shell *shell, char *line)
{
	char	*expanded;
	size_t	i;

	if (!shell || !line)
		return (NULL);
	expanded = ar_strdup(shell->arena, "");
	if (!expanded)
		return (NULL);
	i = 0;
	while (line[i])
	{
		if (line[i] == '$')
		{
			if (expand_dollar_sign(shell, line, &i, &expanded) == -1)
				return (NULL);
		}
		else
		{
			expanded = ar_add_char_to_str(shell->arena, expanded, line[i]);
			if (!expanded)
				return (NULL);
		}
		i++;
	}
	return (expanded);
}

/**
** strip_heredoc_delimiter_quotes - Remove all quotes from heredoc delimiter
**
** BASH BEHAVIOR:
** - Quotes are removed from delimiter: "$USER" → $USER
** - But their presence disables variable expansion in content
**
** EXAMPLES:
**   "$USER" → $USER
**   'EOF'   → EOF
**   E"O"F   → EOF
**
**   delimiter - Original delimiter string (may contain quotes)
**
**   Returns: Arena-allocated string with quotes removed
*/
char	*strip_heredoc_delimiter_quotes(t_arena *arena, char *delimiter)
{
	char	*result;
	int		i;
	int		j;

	if (!delimiter)
		return (NULL);
	result = ar_alloc(arena, ft_strlen(delimiter) + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (delimiter[i])
	{
		if (delimiter[i] != '\'' && delimiter[i] != '"')
			result[j++] = delimiter[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

/**
** generate_filename - Create unique temporary filename for heredoc
**
** FILENAME STRATEGY:
** - Uses /tmp directory (standard Unix temporary location)
** - Includes shell PID for uniqueness across shell instances
** - Includes counter for uniqueness within shell session
** - Format: /tmp/.minishell_heredoc_PID_COUNTER
**
** BENEFITS:
** - /tmp typically has fast filesystem (tmpfs/ramdisk)
** - /tmp cleaned automatically on reboot
** - Proper permissions (0600) for security
**
**   shell - Shell state containing PID and counter
**
**   Returns: Arena-allocated filename string
*/
char	*generate_filename(t_shell *shell)
{
	char	*filename;
	char	*pid_str;
	char	*counter_str;

	if (!shell || !shell->arena)
		return (NULL);
	pid_str = ar_itoa(shell->arena, shell->shell_pid);
	counter_str = ar_itoa(shell->arena, shell->heredoc_counter++);
	if (!pid_str || !counter_str)
		return (NULL);
	filename = ar_strdup(shell->arena, "/tmp/.minishell_heredoc_");
	filename = ar_strjoin(shell->arena, filename, pid_str);
	filename = ar_strjoin(shell->arena, filename, "_");
	filename = ar_strjoin(shell->arena, filename, counter_str);
	return (filename);
}

/**
** write_heredoc_line - Write line to heredoc file with newline
**
** LINE WRITING: Write line content + newline to preserve formatting
** ERROR HANDLING: Check write() return value for disk full, etc.
**
**   fd   - File descriptor to write to
**   line - Line content to write
**
**   Returns: 0 on success, 1 on write failure
*/
int	write_heredoc_line(int fd, char *line)
{
	size_t	len;

	if (!line)
		return (0);
	len = ft_strlen(line);
	/* Write line content */
	if (write(fd, line, len) != (ssize_t)len)
	{
		perror("minishell: heredoc: write failed");
		return (1);
	}
	/* Write newline to preserve line structure */
	if (write(fd, "\n", 1) != 1)
	{
		perror("minishell: heredoc: write failed");
		return (1);
	}
	return (0);
}
