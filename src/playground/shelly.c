#include "minishell.h"

int	shelly(void)
{
	char	*line;

	while (1337)
	{
		// Prompt user for input
		line = readline("mini-shelly> ");
		if (!line) // Handle Ctrl+D (EOF)
		{
			ft_printf("Bye, Bye, exit\n");
			break;
		}
		if (*line) // If line is not empty, add to history
			add_history(line);

		// Example commands to test Readline functions
		if (ft_strcmp(line, "clear") == 0)
		{
			rl_replace_line("", 0);  // Replace current line with ""
			rl_on_new_line();        // Move to new line
			rl_redisplay();          // Redisplay prompt
			ft_printf("\n[Line cleared]\n");
		}
		else if (ft_strcmp(line, "history") == 0)
		{
			// Get the history list from readline library
			HIST_ENTRY **the_list = history_list();
			if (the_list)
			{
				// Print each command in the history with its number
				for (int i = 0; the_list[i]; i++)
					ft_printf("%d: %s\n", i + history_base, the_list[i]->line);
			}
		}
		else if (ft_strcmp(line, "clear_history") == 0)
		{
			rl_clear_history(); // Completely clear history
			ft_printf("[History cleared]\n");
		}
		else if (ft_strcmp(line, "exit") == 0)
		{
			free(line);
			break;
		}
		else
			ft_printf("You typed: %s\n", line);

		free(line); // Free readline buffer
	}
	return (0);
}

