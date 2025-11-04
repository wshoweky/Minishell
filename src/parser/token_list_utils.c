/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gita <gita@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 00:00:52 by gita              #+#    #+#             */
/*   Updated: 2025/11/04 00:00:53 by gita             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	token->was_quoted = 0;
	token->next = NULL;
	return (token);
}

t_token_type	get_token_type(char *str)
{
	if (!str)
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
