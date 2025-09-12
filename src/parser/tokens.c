#include "minishell.h"

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
		new = new_node(words[i]);
		if (!new)
		{
			free_list(head);
			free_split(words);
			return (NULL);
		}
		add_to_end(&head, new);
		i++;
	}
	free_split(words);
	return (head);
}
