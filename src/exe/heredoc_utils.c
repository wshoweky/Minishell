#include "minishell.h"

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
** get_random_value - Get a random value from /dev/urandom
**
** RANDOMNESS SOURCE:
**   Reads from /dev/urandom to get unpredictable values.
**   This ensures unique filenames across multiple shell instances.
**
** RETURNS:
**   Positive random value on success, or a fallback value on error
*/
static unsigned int	get_random_value(void)
{
	int				fd;
	unsigned int	value;
	ssize_t			bytes_read;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return ((unsigned int)12345);
	bytes_read = read(fd, &value, sizeof(value));
	close(fd);
	if (bytes_read != sizeof(value))
		return ((unsigned int)12345);
	return (value);
}

/**
** generate_filename - Create unique temporary filename for heredoc
**
** FILENAME STRATEGY:
** - Uses /tmp directory (standard Unix temporary location)
** - Combines random value from /dev/urandom with incrementing counter
** - Format: /tmp/.minishell_heredoc_[random]_[counter]
**
** UNIQUENESS:
** - Random value prevents collision between concurrent shell instances
** - Counter ensures uniqueness within a single shell session
** - /dev/urandom provides cryptographically strong randomness
**
** BENEFITS:
** - /tmp typically has fast filesystem (tmpfs/ramdisk)
** - /tmp cleaned automatically on reboot
** - Proper permissions (0600) for security
**
**   shell - Shell state containing counter
**
**   Returns: Arena-allocated filename string
*/
char	*generate_filename(t_shell *shell)
{
	char			*filename;
	char			*random_str;
	char			*counter_str;
	unsigned int	random_value;

	if (!shell || !shell->arena)
		return (NULL);
	random_value = get_random_value();
	random_str = ar_itoa(shell->arena, (int)random_value);
	counter_str = ar_itoa(shell->arena, shell->heredoc_counter++);
	if (!random_str || !counter_str)
		return (NULL);
	filename = ar_strdup(shell->arena, "/tmp/.minishell_heredoc_");
	filename = ar_strjoin(shell->arena, filename, random_str);
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
	if (write(fd, line, len) != (ssize_t)len)
	{
		perror("minishell: heredoc: write failed");
		return (1);
	}
	if (write(fd, "\n", 1) != 1)
	{
		perror("minishell: heredoc: write failed");
		return (1);
	}
	return (0);
}
