/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:42:55 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/03 20:12:36 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
static int	setup_single_redirection(t_redir *redir, t_cmd *cmd)
{
	if (!redir || !redir->filename)
		return (-1);
	if (redir->tok_type == TOKEN_REDIRECT_OUT)
		return (handle_output_redirection(redir->filename, 0));
	else if (redir->tok_type == TOKEN_APPEND)
		return (handle_output_redirection(redir->filename, 1));
	else if (redir->tok_type == TOKEN_REDIRECT_IN)
		return (handle_input_redirection(redir->filename));
	else if (redir->tok_type == TOKEN_HEREDOC)
		return (handle_heredoc_file(cmd->heredoc_filename));
	return (-1);
}

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
		if (setup_single_redirection(redir, cmd) != 0)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

/*
** handle_input_redirection - Handle input redirection (<)
**
** DESCRIPTION:
**   Opens file for reading and redirects stdin to it.
**   DEFENSIVE: Ensures FD is always closed on all paths.
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

	fd = -1;
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
		if (fd >= 0)
			close(fd);
		return (-1);
	}
	if (fd >= 0)
		close(fd);
	return (0);
}

/*
** handle_output_redirection - Handle output redirection (>, >>)
**
** DESCRIPTION:
**   Opens file for writing and redirects stdout to it.
**   DEFENSIVE: Ensures FD is always closed on all paths.
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

	fd = -1;
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
		if (fd >= 0)
			close(fd);
		return (-1);
	}
	if (fd >= 0)
		close(fd);
	return (0);
}

/*
** handle_heredoc_file - Handle heredoc redirection using temporary file
**
** DESCRIPTION:
**   Opens the temporary heredoc file for reading and redirects stdin to it.
**   The temporary file was created during parsing by handle_heredocs().
**   DEFENSIVE: Ensures FD is always closed on all paths.
**
** PARAMETERS:
**   filename - Path to temporary heredoc file
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	handle_heredoc_file(char *filename)
{
	int	fd;

	fd = -1;
	if (!filename)
	{
		print_error("minishell", "heredoc", "no temporary file");
		return (-1);
	}
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("minishell: heredoc");
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("minishell: dup2");
		if (fd >= 0)
			close(fd);
		return (-1);
	}
	if (fd >= 0)
		close(fd);
	return (0);
}
