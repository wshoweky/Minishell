/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 20:55:02 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/04 14:23:37 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXE_H
# define EXE_H

// Type aliases for 42 norm compliance
typedef struct stat			t_stat;
typedef struct s_cmd_table	t_cmd_table;
typedef struct s_cmd		t_cmd;
typedef struct s_redir		t_redir;
typedef struct s_shell		t_shell;
typedef struct s_var		t_var;

// Main execution dispatcher
int		exe_cmd(t_shell *shell, t_cmd_table *cmd_table);
int		exe_single_cmd(t_shell *shell, t_cmd *cmd);
int		exe_builtin_with_fork(t_cmd *cmd, t_shell *shell);
int		is_non_forkable_builtin(char *cmd_name);
int		exe_external_cmd(t_shell *shell, t_cmd *cmd);
int		exe_redirection_only(t_shell *shell, t_cmd *cmd);
int		dispatch_builtin(t_cmd *cmd, t_shell *shell);

// Built-in command detection and dispatch
int		is_builtin(char *cmd);
int		exe_builtin(t_cmd *cmd, t_shell *shell);

// Individual built-in implementations
int		builtin_echo(t_cmd *cmd);
int		nl_flag_acceptable(char *cmd_av, int *newline);
int		builtin_pwd(t_cmd *cmd);
int		builtin_cd(t_shell *shell, t_cmd *cmd);
int		builtin_env(t_shell *shell);
int		builtin_export(t_shell *shell, t_cmd *cmd);
int		builtin_unset(t_shell *shell, t_cmd *cmd);
int		builtin_exit(t_shell *shell, t_cmd *cmd);

// Export helper functions
int		copy_vars_fr_env_to_export_list(t_shell *shell);
int		plain_export(t_shell *shell);
int		export_this_var(t_shell *shell, char *arg);
int		find_name_and_value(t_shell *shell, char *arg, t_var **var);
int		this_is_name(t_shell *shell, char *arg, size_t *i, t_var **var);
int		this_is_value(t_shell *shell, char *arg, size_t *i, t_var **var);
int		copy_var_fr_arena_to_shell(t_var *arena_var, t_var *shell_var);
int		register_to_shell_vars(t_shell *shell, t_var *var);
int		destined_to_be_first(t_shell *shell, t_var *var);
int		update_this_fella(t_var *var, t_var *find, t_var *behind);

// Unset helper functions
int		unset_this_var(t_shell *shell, char *input);
void	unset_from_shell_vars(t_shell *shell, char *name);
void	perform_exorcism_on_doppelganger(t_shell *shell, char *name);

// Executable path resolution
char	*find_executable(t_shell *shell, char *cmd);
int		is_executable(char *path);
int		is_regular_file(char *path);
int		is_directory(char *path);
char	*build_path(t_shell *shell, char *dir, char *file);

// Process creation and management
void	execute_child_process(t_shell *shell, t_cmd *cmd, char *path);
int		wait_and_get_status(pid_t pid);

// Redirection handling
int		setup_redirections(t_cmd *cmd);
int		handle_input_redirection(char *filename);
int		handle_output_redirection(char *filename, int append);

// Pipeline execution
void	execute_pipeline(t_shell *shell, t_cmd_table *cmd_table);
int		**alloc_pipe_array(t_shell *shell, int cmd_count);
void	wait_all_children(t_shell *shell, int cmd_count);
void	close_unused_pipes(t_shell *shell, int pipes_to_close);

// Shell initialization and management
t_shell	*init_shell(int ac, char **av, char **env);
void	free_shell(t_shell *shell);
void	free_list_of_vars(t_var **var);
void	free_1_var(t_var **var);
void	free_partial_env(t_shell *shell, int count);

// Shell environment management
char	*get_shell_env_value(t_shell *shell, char *name);
int		set_shell_env_value(t_shell *shell, char *name, char *value);
int		unset_shell_env_value(t_shell *shell, char *name);

// Shell utility functions
char	*create_env_string(char *name, char *value);
void	print_error(char *prefix, char *cmd, char *msg);
int		find_env_index(t_shell *shell, char *name);
int		resize_env_if_needed(t_shell *shell);
int		update_shell_cwd(t_shell *shell, char *old_dir);

// Heredoc utilities
int		handle_heredocs(t_shell *shell, t_cmd_table *cmd_table);
int		handle_heredoc_file(char *heredoc_filename);
int		process_heredoc_input(t_shell *shell, t_redir *redir, char *filename);
int		collect_heredoc_input(t_shell *shell, t_redir *redir, int fd);
int		expand_dollar_sign(t_shell *shell, char *input, size_t *i, char **text);
char	*generate_filename(t_shell *shell);

//	Heredoc functions
char	*expand_heredoc_line(t_shell *shell, char *line);
char	*generate_filename(t_shell *shell);
int		write_heredoc_line(int fd, char *line);
char	*special_heredoc_delimiter(t_arena *arena, char *delimiter);
int		parse_special_delimiter(t_arena *arena, char *delimiter, char **result);
int		check_delimiter_match(char *line, char *delimiter);
void	cleanup_heredoc_files(t_cmd_table *cmd_table);

// Signal handling
int		setup_signal_handlers(void);
void	handle_sigint(int signo);
void	handle_signal_status(t_shell *shell);
void	restore_interactive_signals(void);
void	reset_signals_for_child(void);
int		disable_echoctl(void);
int		heredoc_event_hook(void);
int		handle_heredoc_interrupt(t_shell *shell, char *line);
void	setup_heredoc_signals(void);
void	handle_heredoc_sigint(int signum);

#endif
