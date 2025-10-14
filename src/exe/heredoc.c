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