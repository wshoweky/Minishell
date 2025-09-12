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

//test playground
int	shelly(void);

#endif