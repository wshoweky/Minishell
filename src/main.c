#include "minishell.h"

static const char	*get_colored_prompt(void);
static void			shell_loop(t_shell *shell);
static int			process_input(t_shell *shell, char *input);

// static void	print_tokens(t_tokens *head)
// {
// 	while (head)
// 	{
// 		ft_printf("Token: '%s' | Type: %s", head->value,
// 			get_token_type_name(head->type));
// 		ft_printf("\n");
// 		head = head->next;
// 	}
// }

int	main(int ac, char **av, char **env)
{
	t_shell	*shell;
	int		exit_status;

	shell = init_shell(ac, av, env);
	if (!shell)
	{
		ft_putstr_fd("Error initializing shell\n", STDERR_FILENO);
		return (1);
	}
	if (setup_signal_handlers() == -1)
	{
		ft_putstr_fd("Error setting up signal handlers\n", STDERR_FILENO);
		free_shell(shell);
		return (1);
	}
	while (1337)
		shell_loop(shell);
	exit_status = shell->last_exit_status;
	free_shell(shell);
	return (exit_status);
}

static void	shell_loop(t_shell *shell)
{
	char	*input;

	g_signal = 0;
	input = readline(get_colored_prompt());
	if (!input)
	{
		ft_printf("exit\n");
		exit(shell->last_exit_status);
	}
	handle_signal_status(shell);
	if (*input)
		add_history(input);
	process_input(shell, input);
	free(input);
	ar_reset(shell->arena);
}

static int	process_input(t_shell *shell, char *input)
{
	t_tokens	*tokens;
	t_cmd_table	*cmd_table;

	tokens = tokenize_input(shell->arena, input);
	if (!tokens)
		return (1);
	// print_tokens(tokens); // Debug print of tokens
	cmd_table = register_to_table(shell, tokens);
	if (cmd_table)
	{
		exe_cmd(shell, cmd_table);
		// ft_printf("Command executed with exit status: %d\n",
		// 	shell->last_exit_status); // For debugging
	}
	return (0);
}

/*
** get_colored_prompt - Create a colorful shell prompt
**   Returns static string with ANSI color codes
*/
static const char	*get_colored_prompt(void)
{
	return ("🎃\001\033[31;5m\002SpookyShell\001\033[0m\002"
		"\001\033[5m\002👻\001\033[0m\002> ");
}
