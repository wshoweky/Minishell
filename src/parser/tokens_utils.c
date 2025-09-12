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

t_tokens	*new_node(char *word)
{
	t_tokens	*node;

	node = malloc(sizeof(t_tokens));
	if (!node)
		return (NULL);
	node->value = ft_strdup(word);
	if (!node->value)
	{
		free(node);
		return (NULL);
	}
	node->next = NULL;
	return (node);
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