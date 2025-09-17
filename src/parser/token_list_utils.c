#include "minishell.h"

void	free_split(char **words)
{
	int	wc;

	wc = 0;
	if (!words)
		return ;
	while (words[wc])
	{
		free(words[wc]);
		wc++;
	}
	free(words);
}

t_tokens	*create_token(char *word)
{
	t_tokens	*token;

	token = ft_calloc(1, sizeof(t_tokens));
	if (!token)
		return (NULL);
	token->value = ft_strdup(word);
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->type = get_token_type(word);
	return (token);
}

void	free_list_nodes(t_tokens *head)
{
	t_tokens	*tmp;

	while (head)
	{
		tmp = head->next;
		free(head->value);
		free(head);
		head = tmp;
	}
}

int	list_size(t_tokens *head)
{
	int	size;

	size = 0;
	while (head)
	{
		size++;
		head = head->next;
	}
	return (size);
}

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

t_tokens	*split_commands(char *input)
{
	int	i;
	char	**words;
	t_tokens	*head;
	t_tokens	*new;

	words = ft_split(input, ' ');
	if (!words)
		return (NULL);
	head = NULL;
	i = 0;
	while (words[i])
	{
		new = create_token(words[i]);
		if (!new)
		{
			free_list_nodes(head);
			free_split(words);
			return (NULL);
		}
		add_to_end(&head, new);
		i++;
	}
	free_split(words);
	return (head);
}