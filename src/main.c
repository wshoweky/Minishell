#include "minishell.h"

int	main(void)
{
	shelly();
	return (0);
}
/*
int	main(int ac, char **av, char **env)
{
	char	*input;
	//char	**split_command;

	(void)ac;
	(void)av;
	(void)env;
	while (1337)
	{
		input = readline("WGshell> ");
		if (!input)
		{
			ft_printf("exit\n");
			break;
		}
		if (*input)
			add_history(input);
		
		// TODO: Implement proper parsing and execution
		// split_command = ft_split(input, ' ');
		// tokenization_func(split_command);
		// command_identification(split_command);
		// pipex
		
		if (ft_strcmp(input, "exit") == 0)
		{
			free(input);
			break;
		}
		ft_printf("You typed: %s\n", input);
		free(input);		
	}
	return (0);
}

tokenization_func
{
	char *text = {Makefile, cat, ls, -l, out}
	char *redirection = {<, >}
	char *pipe = {|}
}
*/