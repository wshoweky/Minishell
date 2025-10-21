#include "minishell.h"

/*
** HEREDOC IMPLEMENTATION STRATEGY:
**
** Why Temporary Files Instead of Buffers?
** =======================================
**
** 1. NO SIZE LIMITS: Files can handle gigabytes, buffers are limited (64KB)
** 2. MEMORY EFFICIENCY: Only current line in RAM, rest on disk
** 3. REAL SHELL BEHAVIOR: Bash/Zsh use temporary files for large heredocs
** 4. CLEAN ARCHITECTURE: File-based approach integrates well with redirections
**
** Implementation Flow:
** 1. During parsing: Create temporary file for each heredoc
** 2. Collect user input: Write directly to temporary file
** 3. During execution: Use temporary file as input redirection
** 4. After execution: Clean up temporary files with unlink()
*/
/**
** handle_heredocs - Process all heredocs in command table
**
** HEREDOC PROCESSING FLOW:
** 1. Find all TOKEN_HEREDOC redirections in all commands
** 2. For each heredoc: create temporary file and collect user input
** 3. Store filename in cmd->heredoc_filename for later use
** 4. Return success/failure status
**
** WHY BEFORE EXECUTION: Heredocs must be processed during parsing
** because they require interactive user input before command execution.
**
**   shell     - Shell state with heredoc counter
**   cmd_table - Command table to process
**
**   Returns: 0 on success, 1 on failure
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

/**
** process_heredoc_input - Create temporary file and collect user input
**
** FILE CREATION AND INPUT COLLECTION:
** 1. Create temporary file with write permissions (0644)
** 2. Collect user input line by line using readline
** 3. Write each line to file until delimiter is found
** 4. Close file (will be reopened for reading during execution)
**
** ERROR HANDLING: If file creation fails, report to stderr
**
**   shell    - Shell state for variable expansion
**   redir    - Redirection with delimiter (redir->filename)
**   filename - Path to temporary file
**
**   Returns: 0 on success, 1 on failure
*/
int	process_heredoc_input(t_shell *shell, t_redir *redir, char *filename)
{
	int	fd;
	int	status;

	if (!shell || !redir || !filename)
		return (1);
	/* Create temporary file for writing heredoc content */
	fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd == -1)
	{
		perror("minishell: heredoc: failed to create temporary file");
		return (1);
	}
	/* Collect user input until delimiter is found */
	status = collect_heredoc_input(shell, redir, fd);
	close(fd);
	return (status);
}

/**
** collect_heredoc_input - Interactive input collection for heredoc
**
** INPUT COLLECTION LOOP:
** 1. Display "> " prompt (standard heredoc prompt)
** 2. Read line with readline (handles EOF gracefully)
** 3. Check if line matches delimiter (exact match)
** 4. If not delimiter: expand variables and write to file
** 5. Continue until delimiter found or EOF
**
** VARIABLE EXPANSION: Heredocs support $VAR expansion (like bash)
** EOF HANDLING: Warn about missing delimiter (bash behavior)
**
**   shell - Shell state for variable expansion
**   redir - Redirection with delimiter
**   fd    - File descriptor to write to
**
**   Returns: 0 on success, 1 on EOF without delimiter
*/
int	collect_heredoc_input(t_shell *shell, t_redir *redir, int fd)
{
	char	*line;
	char	*expanded_line;
	char	*delimiter;

	delimiter = redir->filename;
	while (1337)
	{
		/* Display heredoc prompt */
		line = readline("> ");
		/* Handle EOF (Ctrl+D) */
		if (!line)
		{
			ft_printf("minishell: warning: here-document delimited by ");
			ft_printf("end-of-file (wanted `%s')\n", delimiter);
			return (1);
		}
		/* Check if line matches delimiter exactly */
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			return (0);
		}
		/* Expand variables only if delimiter was NOT quoted */
		if (redir->expand_heredoc)
			expanded_line = expand_heredoc_line(shell, line);
		else
			expanded_line = ar_strdup(shell->arena, line);
		if (!expanded_line)
		{
			free(line);
			return (1);
		}
		/* Write line (expanded or literal) to temporary file */
		if (write_heredoc_line(fd, expanded_line) != 0)
		{
			free(line);
			return (1);
		}
		free(line);
	}
}
