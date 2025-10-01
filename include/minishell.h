#ifndef MINISHELL_H
# define MINISHELL_H

// Token types
typedef enum e_token_type
{
	TOKEN_WORD,        // Regular words: echo, hello, file.txt
	TOKEN_PIPE,        // |
	TOKEN_REDIRECT_IN, // <
	TOKEN_REDIRECT_OUT,// >
	TOKEN_APPEND,      // >>
	TOKEN_HEREDOC,     // <<
	TOKEN_EOF          // End of input
}	t_token_type;

typedef struct s_tokens
{
	t_token_type	type;       // Type of token
	char			*value;	
	struct s_tokens	*next;
}	t_tokens;

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

/*
// Shell state structure
typedef struct s_shell
{
	char	**env;				// Our own copy of environment variables
	int		env_capacity;		// Current capacity of env array
	int		env_count;			// Current number of env variables
	int		last_exit_status;	// Exit status of last command ($?)
	char	*cwd;				// Current working directory
	char	*oldpwd;			// Previous working directory (for cd -)
	char	*home;				// HOME directory path
	char	*user;				// Current user name
	char	*shell_name;		// Shell executable path ($0)
	int		shell_pid;			// Shell process ID ($$)
	int		is_interactive;		// Interactive mode flag
	int		should_exit;		// Exit flag for main loop
} t_shell;
*/

//# include <string.h>   // strlen, strcpy, strdup, etc.
# include <unistd.h>   // write, read, close, fork, execve, pipe
# include <stdlib.h>   // malloc, free, exit
# include <stddef.h> // for size_t
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
# include "exe.h"			 // execution
# include "../libft/libft.h"

// Playground functions
//int	shelly(void);

// List utility functions
// int			list_size(t_tokens *head);
// void		free_split(char **words);
// void		free_list_nodes(t_tokens *head);
void		add_to_end(t_tokens **head, t_tokens *new_node);
t_tokens	*create_token(t_arena *arena, char *word);
// t_tokens	*split_commands(t_arena *arena, char *input);

// Tokenization functions
t_tokens		*tokenize_input(t_arena *arena, char *input);
t_tokens		*process_single_token(t_arena *arena, char *input, int *i, t_tokens **head);
char			*extract_next_token(t_arena *arena, char *input, int *i);
char			*check_for_quoted_string(t_arena *arena, char *str);


// Token extraction functions
char			*extract_special_token(t_arena *arena, char *input, int *i);
char			*extract_pipe_token(t_arena *arena, char *input, int *i);
char			*extract_redirect_in_token(t_arena *arena, char *input, int *i);
char			*extract_redirect_out_token(t_arena *arena, char *input, int *i);

// Token utility functions
t_token_type	get_token_type(char *str);
char			*get_token_type_name(t_token_type type);
// int				is_special_char(char c);
void			skip_whitespace(char *input, int *i);

// Parsing functions
t_cmd_table	*register_to_table(t_arena *arena, t_tokens *list_of_toks);
t_cmd		*new_cmd_alloc(t_arena *arena);
int			add_argv(t_arena *arena, t_cmd *command, char *expansion);
int	    	expand_variable_name(t_arena *arena, t_tokens *word_tok);
char    	*find_var_value(char *name);
void	    *err_msg_n_return_null(char *msg);
int		    err_msg_n_return_value(char *msg, int value);

//	Redirection functions
int			is_redirection(t_token_type check);
int			make_redir(t_arena *arena, t_tokens *curr_tok, t_cmd *curr_cmd);
void		set_redir_type(t_token_type tok_type, t_token_type *redir_type);

#endif
