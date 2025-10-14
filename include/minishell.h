#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>    // printf, perror
# include <fcntl.h>    // open
# include <signal.h>   // signals
# include <time.h>     // time sleep usleep
# include <errno.h>    // ENOENT
# include <sys/wait.h> // wait, waitpid
# include <sys/stat.h> // stat lstat fstat
# include <readline/readline.h> // readline()
# include <readline/history.h> // add_history()
# include "arena.h"			// memory arena
# include "../libft/libft.h"
# include "exe.h"			 // execution

// Token types
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
	TOKEN_EOF
}	t_token_type;

typedef struct s_tokens
{
	t_token_type	type;
	char			*value;	
	struct s_tokens	*next;
}	t_tokens;

// Command table structs
typedef struct s_redir
{
	t_token_type	tok_type;
	char			*filename;
	struct s_redir	*next;
}	t_redir;

// Command structure
typedef struct	s_cmd
{
	char			**cmd_av;
	t_redir			*redirections;  //to be used if there is redirections
	char			*heredoc_filename; // Temporary file for heredoc input
	struct s_cmd	*next_cmd;		//to be used if there is pipe
} t_cmd;

// Command table structure
typedef struct	s_cmd_table
{
	int		cmd_count;
	t_cmd	*list_of_cmds;
}	t_cmd_table;

typedef struct s_var
{
	char	*name;
	char	*value;
	int		push_to_env;
	t_var	*next_var;
}	t_var;

// Shell state structure
typedef struct s_shell
{
	// Environment variables
	char	**env;				// Our own copy of environment variables
	int		env_capacity;		// Current capacity of env array
	int		env_count;			// Current number of env variables
	
	// Exit status
	int		last_exit_status;	// Exit status of last command ($?)
	
	// Directory paths
	char	*cwd;				// Current working directory
	char	*oldpwd;			// Previous working directory (for cd -)
	char	*home;				// HOME directory path
	
	// Shell metadata
	char	*user;				// Current user name
	char	*shell_name;		// Shell executable path ($0)
	int		shell_pid;			// Shell process ID ($$)
	
	// Control flags
	int		is_interactive;		// Interactive mode flag
	int		should_exit;		// Exit flag for main loop
	
	// Memory management
	t_arena	*arena;				// Memory arena for temporary allocations
	
	// Pipeline execution state
	int		**pipe_array;		// Array of pipe file descriptors [cmd_count-1][2]
	int		*pipe_pids;			// Array of child process PIDs
	int		children_forked;	// Number of children successfully forked
	
	// Heredoc support
	//int		heredoc_counter;	// Counter for unique heredoc filenames
	t_var	*vars;
} t_shell;

// Playground functions
//int	shelly(void);

// Token utility functions
char			*get_token_type_name(t_token_type type);
// int			list_size(t_tokens *head);
// void		free_split(char **words);
// void		free_list_nodes(t_tokens *head);
// t_tokens	*split_commands(t_arena *arena, char *input);

// Tokenization functions

t_tokens		*tokenize_input(t_arena *arena, char *input);
void			skip_whitespace(char *input, int *i);
t_tokens		*process_single_token(t_arena *arena, char *input, int *i,
					t_tokens **head);
t_tokens		*create_token(t_arena *arena, char *word);
t_token_type	get_token_type(char *str);
void			add_to_end(t_tokens **head, t_tokens *new_node);

// Parsing input for correct tokenization functions

char			*extract_next_token(t_arena *arena, char *input, int *i);
int				chop_up_input(t_arena *arena, char *input, int *i,
					char **string);
int				char_in_quotes(t_arena *arena, char **string,
					char current_char, int *in_quotes);
int				char_outside_quotes(t_arena *arena, char **string,
					char current_char, int *in_quotes);
int				char_is_quote(t_arena *arena, char **string, char current_char,
					int *in_quotes);
int				char_normal_outside_quotes(t_arena *arena, char **string,
					char current_char);

// Special token extraction functions

int				extract_special_token(t_arena *arena, char **string,
					char current);
int				extract_pipe_token(t_arena *arena, char **string);
int				extract_redirect_in_token(t_arena *arena, char **string);
int				extract_redirect_out_token(t_arena *arena, char **string);

// Quotes in string check and modify functions

char			*check_for_quoted_string(t_arena *arena, char *str);
int				check_for_unclosed_quote(char *str, size_t *i);
int				remove_quotes_for_plain_string(t_arena *arena, char *str,
					char **output, size_t *i);
int				build_output(t_arena *arena, char **output, char c);
int				ensure_output_not_null(t_arena *arena, char **output);

// Parsing functions

t_cmd_table		*register_to_table(t_shell *shell, t_tokens *list_of_toks);
t_cmd			*new_cmd_alloc(t_arena *arena);
void			*err_msg_n_return_null(char *msg);
int				err_msg_n_return_value(char *msg, int value);

// Token checking functions

int				check_current_token(t_shell *shell, t_tokens *token,
					t_cmd **current_cmd, t_cmd_table *table);
int				check_token_word(t_shell *shell, t_tokens *token,
					t_cmd *current_cmd);
int				expand_variable_name(t_shell *shell, t_tokens *word_tok, int in_redir);
int				var_in_redir_outside_2xquotes(char *tok_value);
int				add_argv(t_arena *arena, t_cmd *command, char *expansion);
void			get_old_argv(char **old, char **new, size_t *i);

// Variable name expansion

int				go_thru_input(t_shell *shell, char *input, char **expand_text);
int				dollar_sign_encounter(t_shell *shell, char *input, size_t *i,
					char **text);
int				other_character(t_arena *arena, char **expand_text,
					char current_char, int *in_quote);
int				transform_var_name(t_shell *shell, char **text, char *var_name);

//	Redirection functions

int				is_redirection(t_token_type check);
int				make_redir(t_shell *shell, t_tokens *curr_tok, t_cmd *curr_cmd);
void			set_redir_type(t_token_type tok_type, t_token_type *redir_type);
int				work_on_filename(t_shell *shell, t_tokens *tok_name, char **name);

#endif
