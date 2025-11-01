#include "minishell.h"

static const char	*get_colored_prompt(void);
static void			shell_loop(t_shell *shell);
static int			process_input(t_shell *shell, char *input);
static void			handle_signal_status(t_shell *shell);

static void	print_tokens(t_tokens *head) // debug function to print the token list
{
	while (head)
	{
		ft_printf("Token: '%s' | Type: %s", head->value,
			get_token_type_name(head->type));
		ft_printf("\n");
		head = head->next;
	}
}

int	main(int ac, char **av, char **env)
{
	t_shell	*shell;

	shell = init_shell(ac, av, env);
	if (!shell)
	{
		ft_printf("Failed to initialize shell\n");
		return (1);
	}
	if (setup_signal_handlers() == -1)
	{
		ft_printf("Failed to setup signal handlers\n");
		free_shell(shell);
		return (1);
	}
	while (1337)
		shell_loop(shell);
	free_shell(shell);
	return (shell->last_exit_status);
}

static void	handle_signal_status(t_shell *shell)
{
	if (g_signal == SIGINT)
	{
		shell->last_exit_status = 130;
		g_signal = 0;
	}
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
	if (ft_strcmp(input, "exit") == 0)
	{
		free(input);
		exit(shell->last_exit_status);
	}
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
	{
		//ft_printf("Error in tokenization!\n");
		return (1);
	}
	print_tokens(tokens); // Debug print of tokens
	cmd_table = register_to_table(shell, tokens);
	if (cmd_table)
	{
		exe_cmd(shell, cmd_table);
		ft_printf("Command executed with exit status: %d\n",
			shell->last_exit_status); // For debugging
	}
	return (0);
}

/*
** get_colored_prompt - Create a colorful shell prompt
**   Returns static string with ANSI color codes
*/
static const char	*get_colored_prompt(void)
{
	// Halloween SpookyShell with readline-compatible ANSI codes! 🎃👻
	// \001 and \002 tell readline which parts are non-printable
	// This fixes cursor positioning and line wrapping issues
	
	// FIXED: Readline-compatible blinking SpookyShell prompt
	// \001 = RL_PROMPT_START_IGNORE, \002 = RL_PROMPT_END_IGNORE
	return ("🎃\001\033[31;5m\002SpookyShell\001\033[0m\002"
			"\001\033[5m\002👻\001\033[0m\002> ");
	
	// Alternative options (all readline-compatible):
	// Simple colored: "🎃\001\033[31m\002SpookyShell\001\033[0m\002👻> "
	// With skull: "💀\001\033[31m\002SpookyShell\001\033[0m\002🧙‍♂️> "
	// With bat: "🎃\001\033[31m\002SpookyShell\001\033[0m\002🦇> "
}
