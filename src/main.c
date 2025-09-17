#include "minishell.h"

// follow and update the todo_list.txt ✨

static  void	print_tokens(t_tokens *head)// Debug function to print the token list
{
	while (head)
	{
		ft_printf("Token: '%s' | Type: %s", head->value, get_token_type_name(head->type));
		if (head->was_quoted)
			ft_printf(" | Quoted: Yes");
		ft_printf("\n");
		head = head->next;
	}
}

int	main(int ac, char **av, char **env)
{
	char	*input;
	t_tokens	*tokens;
	t_cmd_table	*cmd_table;

	(void)ac;
	(void)av;
	(void)env;
	while (1337)
	{
		input = readline("WGshell> ");  // Allocates memory for input, must be freed
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
		tokens = tokenize_input(input);
		if (!tokens)
		{
			ft_printf("Error in tokenization!\n");
			free(input);
		}
		else
		{
			print_tokens(tokens);
			cmd_table = register_to_table(tokens); //Checking if command table works
			free_list_nodes(tokens);
			free(input);
		}
	}
	return (0);
}
