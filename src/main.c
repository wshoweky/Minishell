#include "minishell.h"

int	main(int ac, char **av, char **env)
{
	char	*input;
	char	**split_command;

	while (not exit)
	{
		//ft_printf("> "); //might not need to print this separately
		input = readline("WGshell> "); //will print the prompt and get user input at the same time
		//what if malloc in readline fail? what if EOF fund?
		if (input != NULL) //if a valid user input or non empty string
		{
			add_history(input); //readline stuff, remember strings itself
		}
		
		split_command = ft_split(input, isspace());
		tokenization_func(split_command);
		command_identification(text fr tokenization);
		pipex
		free (input); //readline malloc -> needs to free
	}
	

	return (0);
}


tokenization_func
{
	char *text = {Makefile, cat, ls, -l, out}
	char *redirection = {<, >}
	char *pipe = {|}
}