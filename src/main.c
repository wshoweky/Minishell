#include "minishell.h"

static const char	*get_colored_prompt(void);

// static  void	print_tokens(t_tokens *head)// Debug function to print the token list
// {
// 	while (head)
// 	{
// 		ft_printf("Token: '%s' | Type: %s", head->value, get_token_type_name(head->type));
// 		ft_printf("\n");
// 		head = head->next;
// 	}
// }

int	main(int ac, char **av, char **env)
{
	char		*input;
	t_tokens	*tokens;
	t_cmd_table	*cmd_table;
	t_shell		*shell;
	int			exit_code;

	shell = init_shell(ac, av, env);
	if (!shell)
	{
		ft_printf("Failed to initialize shell\n");
		return (1);
	}
	disable_echoctl();
	if (setup_signal_handlers() == -1)
	{
		ft_printf("Failed to set up signal handlers\n");
		free_shell(shell);
		return (1);
	}
	while (1337)
	{
		input = readline(get_colored_prompt());  // Allocates memory for input, must be freed
		if (!input)
		{
			ft_printf("exit\n");
			break;
		}
		if (*input)
			add_history(input);	// History cleanup is readline's responsibility, no need to free manually
		if (ft_strcmp(input, "exit") == 0)
		{
			free(input);
			break;
		}
		tokens = tokenize_input(shell->arena, input);
		if (!tokens)
		{
			// ft_printf("Error in tokenization!\n");
			free(input);
		}
		else
		{
			// print_tokens(tokens);
			cmd_table = register_to_table(shell, tokens);
			if (cmd_table)
			{
				exe_cmd(shell, cmd_table);
				// ft_printf("Command executed with exit status: %d\n",
				// 	shell->last_exit_status);
			}
			free(input);
		}
		ar_reset(shell->arena);
		g_signal = 0;
	}
	exit_code = shell->last_exit_status;
	free_shell(shell);
	return (exit_code);
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
