#include "minishell.h"

static  void	print_tokens(t_tokens *head)
{
	while (head)
	{
		ft_printf("Token: %s\n", head->value);
		head = head->next;
	}
}

int	main(int ac, char **av, char **env)
{
	char	*input;
	t_tokens	*tokens;

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
		tokens = split_commands(input);
		if (!tokens)
		{
			ft_printf("Error!\n");
			free(input);
		}
		else
		{
			print_tokens(tokens);
			free_list_nodes(tokens);
			free(input);
		}
		//ft_printf("You typed: %s\n", input);
		//free(input);
	}
	return (0);
}
/*
tokenization_func
{
	char *text = {Makefile, cat, ls, -l, out}
	char *redirection = {<, >}
	char *pipe = {|}
}
*/
//shelly test
/*
int	main(void)
{
	shelly();
	return (0);
}
*/