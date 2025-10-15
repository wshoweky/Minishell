#include "minishell.h"

//int handle_heredocs(t_shell *shell, t_cmd_table *cmd_table)
//iterate through all commands in cmd_table
// for each command, check if it has a heredoc redirection
// if it does, create a temporary file for the heredoc content

//int process_heredoc_input(char *delimiter, char *heredoc_filename)
//read lines from stdin until the delimiter is encountered
//write each line to the heredoc_filename file
//close the file and return 0 on success, -1 on error

//char *expand_heredoc_line(t_shell *shell, char *line)
//expand any variables in the line using existing variable expansion logic
//return the expanded line

// int	handle_herdocs(t_shell *shell, t_cmd_table *cmd_table)
// {
// 	t_cmd	*current_cmd;
// 	t_redir	*current_redir;

// 	if (!shell || !cmd_table)
// 		return (1);
// 	current_cmd = cmd_table->list_of_cmds;
// 	while (current_cmd)
// 	{
// 		current_redir = current_cmd->redirections;
// 		while (current_redir)
// 		{
// 			if (current_redir->tok_type == TOKEN_HEREDOC)
// 			{
// 				current_cmd->heredoc_filename = generate_heredoc_filename(shell);
// 				if (!current_cmd->heredoc_filename)
// 					return (1);
// 				if (process_heredoc_input(shell, current_redir,
// 						current_cmd->heredoc_filename) != 0)
// 					return (1);
// 			}
// 			current_redir = current_redir->next;
// 		}
// 		current_cmd = current_cmd->next_cmd;
// 	}
// 	return (0);
// }

// int	process_heredoc_input(t_shell *shell, t_redir *redir, char *filename)
// {
// 	int	fd;
// 	int	status;

// 	if (!shell || !redir || !filename)
// 		return (1);
// 	fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0644); // logical or that add bits for permessions
// 	if (fd == -1)
// 	{
// 		perror ("minishell: heredoc: failed to create temporary file");
// 		return (1);
// 	}
// 	status = collect_heredoc_input(shell, redir, fd);
// 	close (fd);
// 	return (status);
// }

// int	collect_heredoc_input(t_shell *shell, t_redir *redir, int fd)
// {
// 	char	*line;
// 	char	*expanded_line;
// 	char	*delimiter;

// 	delimiter = redir->filename; // EOF
// 	while (1337)
// 	{
// 		line = readline("> ");
// 		//Handle EOF
// 		if (!line)
// 		{
// 			ft_printf("minishell: warning: here-document delimited by ");
// 			ft_printf("endof-file (wanted `%s')\n", delimiter);
// 			return (1);
// 		}
// 		//Check if the line matches delimiter exactly
// 		if (ft_strcmp(line, delimiter) == 0)
// 		{
// 			free(line);
// 			return (0);
// 		}
// 		//Expand variables in heredoc line
// 		expanded_line = expand_heredoc_line(shell, line);
// 		if (!expanded_line)
// 		{
// 			free(line);
// 			return (1);
// 		}
// 		//Write expanded line to temporary file!
// 		if (write_heredoc_line(fd, expanded_line) != 0)
// 		{
// 			free(line);
// 			return (1);
// 		}
// 		free(line);
// 	}
// }