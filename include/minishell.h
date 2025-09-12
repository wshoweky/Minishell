#ifndef MINISHELL_H
# define MINISHELL_H

//# include <string.h>   // strlen, strcpy, strdup, etc.
# include <unistd.h>   // write, read, close, fork, execve, pipe
# include <stdlib.h>   // malloc, free, exit
# include <stdio.h>    // printf, perror
# include <fcntl.h>    // open
# include <signal.h>   // signals
# include <time.h>     // time sleep usleep
# include <errno.h>    // ENOENT
# include <sys/wait.h> // wait, waitpid
# include <sys/stat.h> // stat lstat fstat
# include <readline/readline.h> // readline()
# include <readline/history.h> // add_history()
# include "../libft/libft.h"

typedef struct s_tokens
{
	char	*value;
	struct	s_tokens	*next;
}	t_tokens;

//test playground
//int	shelly(void);

//tokens
int	list_size(t_tokens *head);
void	free_split(char **words);
void	free_list_nodes(t_tokens *head);
void	add_to_end(t_tokens **head, t_tokens *new_node);
t_tokens	*new_node(char *word);
t_tokens	*split_commands(char *input);

#endif