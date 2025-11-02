#include "minishell.h"

/* handle_heredocs - Process all heredocs in command table before execution
** Creates temp files and collects user input for each heredoc.
** Returns: 0 on success, 1 on failure
*/
int	handle_heredocs(t_shell *shell, t_cmd_table *cmd_table)
{
	t_cmd	*current_cmd;
	t_redir	*current_redir;

	if (!shell || !cmd_table)
		return (1);
	current_cmd = cmd_table->list_of_cmds;
	while (current_cmd)
	{
		current_redir = current_cmd->redirections;
		while (current_redir)
		{
			if (current_redir->tok_type == TOKEN_HEREDOC)
			{
				current_cmd->heredoc_filename = generate_filename(shell);
				if (!current_cmd->heredoc_filename)
					return (1);
				if (process_heredoc_input(shell, current_redir,
						current_cmd->heredoc_filename) != 0)
					return (1);
			}
			current_redir = current_redir->next;
		}
		current_cmd = current_cmd->next_cmd;
	}
	return (0);
}

/* process_heredoc_input - Create temp file and collect heredoc input
** Opens file, collects input until delimiter, closes file.
** Returns: 0 on success, 1 on failure
*/
int	process_heredoc_input(t_shell *shell, t_redir *redir, char *filename)
{
	int	fd;
	int	status;

	if (!shell || !redir || !filename)
		return (1);
	fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd == -1)
	{
		perror("minishell: heredoc: failed to create temporary file");
		return (1);
	}
	status = collect_heredoc_input(shell, redir, fd);
	close(fd);
	return (status);
}

/* handle_eof_error - Handle EOF without delimiter in heredoc
** Prints warning and sets exit status to 0 (bash behavior).
*/
static int	handle_eof_error(t_shell *shell, char *delimiter)
{
	if (!g_signal)
	{
		ft_putstr_fd("minishell: warning: here-document delimited by ", 2);
		ft_putstr_fd("end-of-file (wanted `", 2);
		ft_putstr_fd(delimiter, 2);
		ft_putstr_fd("')\n", 2);
	}
	shell->last_exit_status = 0;
	return (1);
}

/* process_heredoc_line - Expand variables if needed and write line to file
** Returns: 0 on success, 1 on failure
*/
static int	process_heredoc_line(t_shell *shell, t_redir *redir, int fd,
		char *line)
{
	char	*expanded_line;

	if (!line || !redir || !shell)
		return (1);
	if (redir->expand_heredoc)
		expanded_line = expand_heredoc_line(shell, line);
	else
		expanded_line = ar_strdup(shell->arena, line);
	if (!expanded_line)
	{
		if (line)
			free(line);
		return (1);
	}
	if (write_heredoc_line(fd, expanded_line) != 0)
	{
		if (line)
			free(line);
		return (1);
	}
	return (0);
}

/* collect_heredoc_input - Read heredoc input until delimiter or EOF/interrupt
** Displays "> " prompt, reads lines, expands variables, writes to file.
** Returns: 0 on success, 1 on EOF/interrupt
*/
int	collect_heredoc_input(t_shell *shell, t_redir *redir, int fd)
{
	char	*line;
	char	*delimiter;

	setup_heredoc_signals();
	delimiter = redir->filename;
	while (1)
	{
		line = readline("> ");
		if (g_signal == SIGINT)
			return (handle_heredoc_interrupt(shell, line));
		if (!line)
		{
			restore_interactive_signals();
			return (handle_eof_error(shell, delimiter));
		}
		if (check_delimiter_match(line, delimiter))
			return (0);
		if (process_heredoc_line(shell, redir, fd, line) != 0)
		{
			restore_interactive_signals();
			return (1);
		}
		free(line);
	}
}
