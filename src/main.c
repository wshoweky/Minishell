#include "minishell.h"
//#include "exe.h"

// follow and update the todo_list.txt ✨

static const char	*get_colored_prompt(void);

static  void	print_tokens(t_tokens *head)// Debug function to print the token list
{
	while (head)
	{
		ft_printf("Token: '%s' | Type: %s", head->value, get_token_type_name(head->type));
		if (head->was_quoted == 1)
			ft_printf(" | Quoted: Single ('') - No expansion");
		else if (head->was_quoted == 2)
			ft_printf(" | Quoted: Double (\"\") - With expansion");
		ft_printf("\n");
		head = head->next;
	}
}

int	main(int ac, char **av, char **env)
{
	char		*input;
	t_tokens	*tokens;
	t_arena		*arena;
	t_cmd_table	*cmd_table;
	int			exit_status;

	(void)ac;
	(void)av;
	exit_status = 0;
	arena = ar_init();
	if (!arena)
	{
		ft_printf("Failed to initialize memory arena\n");
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
		tokens = tokenize_input(arena, input);
		if (!tokens)
		{
			ft_printf("Error in tokenization!\n");
			free(input);
		}
		else
		{
			print_tokens(tokens);
			cmd_table = register_to_table(arena, tokens);
			if (cmd_table)
			{
				// Execute the commands
				exit_status = exe_cmd(arena, cmd_table, env);
				ft_printf("Command executed with exit status: %d\n", exit_status);
			}
			// No need to free tokens or cmd_table as they're in the arena
			free(input);
		}
		// Reset arena for next command
		ar_reset(arena);
	}
	// Cleanup arena before exit
	free_arena(arena);
	return (0);
}


/*
** get_colored_prompt - Create a colorful shell prompt
**   Returns static string with ANSI color codes
*/
static const char	*get_colored_prompt(void)
{
	// Halloween SpookyShell with emojis! 🎃👻
	// \033[31m - Red text for "SpookyShell"
	// \033[0m - Reset colors after text
	
	// Option 1: SpookyShell
	// return ("🎃\033[31mSpookyShell\033[0m👻> ");
	
	// Option 6: BLINKING SpookyShell
	//return ("🎃\033[31;5mSpookyShell\033[0m👻> ");
	
	// Option 7: BOLD BLINKING SpookyShell (SUPER SPOOKY!)
	// return ("🎃\033[31;1;5mSpookyShell\033[0m👻> ");
		
	// Option 9: BOTH BLINKING (ULTIMATE SPOOKY!)
	return ("🎃\033[31;5mSpookyShell\033[0m\033[5m👻\033[0m> ");
	
	// Option 2: More spooky with skull and wizard
	// return ("💀\033[31mSpookyShell\033[0m🧙‍♂️> ");
	
	// Option 3: Classic Halloween with bat
	// return ("🎃\033[31mSpookyShell\033[0m🦇> ");
	
	// Option 4: Spooky with brackets
	// return ("\033[31m[💀SpookyShell💀]\033[0m$ ");
}
