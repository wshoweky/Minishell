#include "minishell.h"

int	main(int ac, char **av, char **env)
{
	char	*input;
	char	**history;
	char	**split_command;
	int i = 0;

	while (not exit)
	{
		ft_printf("> ");
		input = readline() or gnl(fd0);
		history[i] = ft_strdup(input);
		split_command = ft_split(input, isspace());
		tokenization_func(split_command);
		command_identification(text fr tokenization);
		pipex
		i++;
	}
	

	return (0);
}


tokenization_func
{
	char *text = {Makefile, cat, ls, -l, out}
	char *redirection = {<, >}
	char *pipe = {|}
}