#include "minishell.h"

// void	free_split(char **words)
// {
// 	int	wc;

// 	wc = 0;
// 	if (!words)
// 		return ;
// 	while (words[wc])
// 	{
// 		free(words[wc]);
// 		wc++;
// 	}
// 	free(words);
// }

t_tokens	*create_token(t_arena *arena, char *word)
{
	t_tokens	*token;

	token = ar_alloc(arena, sizeof(t_tokens));
	if (!token)
		return (NULL);
	token->value = ar_strdup(arena, word);
	if (!token->value)
		return (NULL);
	token->type = get_token_type(word);
	token->was_quoted = 0;  // Will be set during quote processing
	token->next = NULL;
	return (token);
}

t_token_type	get_token_type(char *str)
{
	if (!str) //fix: don't check for empty string (!*str)
		return (TOKEN_EOF);
	if (ft_strcmp(str, "|") == 0)
		return (TOKEN_PIPE);
	if (ft_strcmp(str, "<") == 0)
		return (TOKEN_REDIRECT_IN);
	if (ft_strcmp(str, ">") == 0)
		return (TOKEN_REDIRECT_OUT);
	if (ft_strcmp(str, ">>") == 0)
		return (TOKEN_APPEND);
	if (ft_strcmp(str, "<<") == 0)
		return (TOKEN_HEREDOC);
	return (TOKEN_WORD);
}

// // to be printed for debugging
// char	*get_token_type_name(t_token_type type)
// {
// 	if (type == TOKEN_WORD)
// 		return ("WORD");
// 	if (type == TOKEN_PIPE)
// 		return ("PIPE");
// 	if (type == TOKEN_REDIRECT_IN)
// 		return ("REDIRECT_IN");
// 	if (type == TOKEN_REDIRECT_OUT)
// 		return ("REDIRECT_OUT");
// 	if (type == TOKEN_APPEND)
// 		return ("APPEND");
// 	if (type == TOKEN_HEREDOC)
// 		return ("HEREDOC");
// 	return ("EOF");
// }

// void	free_list_nodes(t_tokens *head)
// {
// 	t_tokens	*tmp;

// 	while (head)
// 	{
// 		tmp = head->next;
// 		free(head->value);
// 		free(head);
// 		head = tmp;
// 	}
// }

// int	list_size(t_tokens *head)
// {
// 	int	size;

// 	size = 0;
// 	while (head)
// 	{
// 		size++;
// 		head = head->next;
// 	}
// 	return (size);
// }

void	add_to_end(t_tokens **head, t_tokens *new_node)
{
	t_tokens	*tmp;

	if (!*head)
	{
		*head = new_node;
		return ;
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

// t_tokens	*split_commands(t_arena *arena, char *input)
// {
// 	int	i;
// 	char	**words;
// 	t_tokens	*head;
// 	t_tokens	*new;

// 	words = ar_split(arena, input, ' ');
// 	if (!words)
// 		return (NULL);
// 	head = NULL;
// 	i = 0;
// 	while (words[i])
// 	{
// 		new = create_token(arena, words[i]);
// 		if (!new)
// 		{
// 			// No need to free tokens or words as they're in the arena
// 			return (NULL);
// 		}
// 		add_to_end(&head, new);
// 		i++;
// 	}
// 	// No need to free words as they're in the arena
// 	return (head);
// }