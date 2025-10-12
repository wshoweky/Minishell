#include "minishell.h"

static int	setup_single_redirection(t_redir *redir);

/*
** setup_redirections - Setup file redirections for command execution
**
** DESCRIPTION:
**   Sets up input and output redirections using dup2().
**   Called in child process before execve().
**
** PARAMETERS:
**   cmd - Command structure with redirections
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	setup_redirections(t_cmd *cmd)
{
	t_redir	*redir;

	if (!cmd || !cmd->redirections)
		return (0);
	redir = cmd->redirections;
	while (redir)
	{
		if (setup_single_redirection(redir) != 0)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

/*
** setup_single_redirection - Setup a single redirection
**
** DESCRIPTION:
**   Handles a single redirection (>, >>, <, <<).
**   Opens file and redirects using dup2().
**
** PARAMETERS:
**   redir - Single redirection structure
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
static int	setup_single_redirection(t_redir *redir)
{
	if (!redir)
		return (-1);
	if (!redir->filename)
		return (err_msg_n_return_value("Ambiguous redirect\n", -1));
	printf("DEBUG: Setting up redirection: type= %d, file= %s\n",
		redir->tok_type, redir->filename);
	if (redir->tok_type == TOKEN_REDIRECT_OUT) // >
		return (handle_output_redirection(redir->filename, 0));
	else if (redir->tok_type == TOKEN_APPEND) // >>
		return (handle_output_redirection(redir->filename, 1));
	else if (redir->tok_type == TOKEN_REDIRECT_IN) // <
		return (handle_input_redirection(redir->filename));
	else if (redir->tok_type == TOKEN_HEREDOC) // <<
		return (handle_heredoc(redir->filename));
	return (-1); // Unknown redirection type
}

/*
** handle_input_redirection - Handle input redirection (<)
**
** DESCRIPTION:
**   Opens file for reading and redirects stdin to it.
**
** PARAMETERS:
**   filename - Name of file to read from
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	handle_input_redirection(char *filename)
{
	int	fd;

	if (!filename)
		return (-1);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror(filename);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2 stdin");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/*
** handle_output_redirection - Handle output redirection (>, >>)
**
** DESCRIPTION:
**   Opens file for writing and redirects stdout to it.
**
** PARAMETERS:
**   filename - Name of file to write to
**   append   - 1 for append (>>), 0 for truncate (>)
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	handle_output_redirection(char *filename, int append)
{
	int	fd;
	int	flags;

	if (!filename)
		return (-1);
	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(filename, flags, 0644);
	if (fd < 0)
	{
		perror(filename);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/*
** handle_heredoc - Handle heredoc redirection (<<)
**
** DESCRIPTION:
**   Creates temporary file for heredoc content and redirects stdin to it.
**   TODO: Implement heredoc functionality.
**
** PARAMETERS:
**   delimiter - Heredoc delimiter string
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	handle_heredoc(char *delimiter)
{
	(void)delimiter; // Suppress unused parameter warning
	// TODO: Implement heredoc functionality
	ft_printf("minishell: heredoc not yet implemented\n");
	return (-1);
}
