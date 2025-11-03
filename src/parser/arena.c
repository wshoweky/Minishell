/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:45:39 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/03 17:45:42 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_arena	*ar_init(void)
{
	t_arena	*arena;

	arena = ft_calloc(1, sizeof(t_arena));
	if (!arena)
		return (NULL);
	arena->size = AR_SIZE;
	arena->buffer = ft_calloc(1, arena->size);
	if (!arena->buffer)
	{
		free(arena);
		return (NULL);
	}
	arena->offset = 0;
	return (arena);
}

void	*ar_alloc(t_arena *arena, size_t bytes)
{
	void	*ptr;
	t_arena	*current;
	t_arena	*new_arena;

	ptr = NULL;
	if (normally_fit_in_arena(arena, &ptr, &bytes) == 1)
		return (ptr);
	current = arena;
	while (current->next)
	{
		current = current->next;
		if (current->offset + bytes <= current->size)
		{
			ptr = current->buffer + current->offset;
			current->offset += bytes;
			return (ptr);
		}
	}
	new_arena = ar_init();
	if (!new_arena)
		return (NULL);
	current->next = new_arena;
	ptr = new_arena->buffer + new_arena->offset;
	new_arena->offset += bytes;
	return (ptr);
}

int	normally_fit_in_arena(t_arena *arena, void **ptr, size_t *bytes)
{
	*bytes = (*bytes + 7) & ~7;
	if (*bytes > AR_SIZE)
		return (err_msg_n_return_value("Error: Allocation request "
				"exceeds maximum arena size\n", 1));
	if (arena->offset + *bytes <= arena->size)
	{
		*ptr = arena->buffer + arena->offset;
		arena->offset += *bytes;
		return (1);
	}
	return (0);
}

void	free_arena(t_arena *arena)
{
	t_arena	*tmp;

	while (arena)
	{
		tmp = arena->next;
		if (arena->buffer)
		{
			free(arena->buffer);
			arena->buffer = NULL;
		}
		free (arena);
		arena = tmp;
	}
	arena = NULL;
}

void	ar_reset(t_arena *arena)
{
	while (arena)
	{
		ft_bzero(arena->buffer, AR_SIZE);
		arena->offset = 0;
		arena = arena->next;
	}
}
