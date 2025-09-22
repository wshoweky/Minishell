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
# include "arena.h"			 // memory arena
# include "../libft/libft.h"

typedef enum e_token_type
{
	TOKEN_WORD,        // Regular words: echo, hello, file.txt
	TOKEN_PIPE,        // |
	TOKEN_REDIRECT_IN, // <
	TOKEN_REDIRECT_OUT,// >
	TOKEN_APPEND,      // >>
	TOKEN_HEREDOC,     // <<
	TOKEN_VAR,		   // Variable $
	TOKEN_EOF          // End of input
}	t_token_type;

typedef struct s_tokens
{
	t_token_type	type;       // Type of token
	char			*value;	
	int				was_quoted;  // 0: not quoted, 1: single quotes, 2: double quotes
	struct s_tokens	*next;
}	t_tokens;

// Playground functions
int	shelly(void);

// List utility functions
int			list_size(t_tokens *head);
void		free_list_nodes(t_tokens *head);
void		add_to_end(t_tokens **head, t_tokens *new_node);
t_tokens	*create_token(t_arena *arena, char *word);
t_tokens	*split_commands(t_arena *arena, char *input);

// Tokenization functions
t_tokens		*tokenize_input(t_arena *arena, char *input);
t_tokens		*process_single_token(t_arena *arena, char *input, int *i, t_tokens **head);
char			*extract_next_token(t_arena *arena, char *input, int *i, t_tokens **new_token);
char			*extract_pipe_token(t_arena *arena, char *input, int *i);

// Token extraction functions
char			*extract_word_token(t_arena *arena, char *input, int *i);
char			*extract_quoted_token(t_arena *arena, char *input, int *i, t_tokens **new_token);
char			*extract_special_token(t_arena *arena, char *input, int *i);
char			*extract_redirect_in_token(t_arena *arena, char *input, int *i);
char			*extract_redirect_out_token(t_arena *arena, char *input, int *i);
char			*extract_variable_token(t_arena *arena, char *input, int *i, t_tokens **new_token);

// Token utility functions
t_token_type	get_token_type(char *str);
char			*get_token_type_name(t_token_type type);
int				is_special_char(char c);
void			skip_whitespace(char *input, int *i);


// Command table structs
typedef struct	s_redir
{
	t_token_type	tok_type;
	char			*filename;		//to be used if there is redirection
	struct s_redir	*next;
} t_redir;

typedef struct	s_cmd
{
	char			**cmd_av;
	t_redir			*redirections;  //to be used if there is redirections
	struct s_cmd	*next_cmd;		//to be used if there is pipe
} t_cmd;

typedef struct	s_cmd_table
{
	int		cmd_count;
	t_cmd	*list_of_cmds;
} t_cmd_table;

t_cmd_table	*register_to_table(t_arena *arena, t_tokens *list_of_toks);
t_cmd		*new_cmd_alloc(t_arena *arena);
int			is_redirection(t_token_type check);
int			make_redir(t_arena *arena, t_tokens *curr_tok, t_cmd *curr_cmd);
void		set_redir_type(t_token_type tok_type, t_token_type *redir_type);
int			add_argv(t_arena *arena, t_cmd *command, char *expansion);
void	    *err_msg_n_return_null(char *msg);

#endif
