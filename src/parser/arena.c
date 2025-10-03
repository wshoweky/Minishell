/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:39:50 by wshoweky          #+#    #+#             */
/*   Updated: 2025/10/03 18:05:36 by wshoweky         ###   ########.fr       */
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

/*
if (bytes > AR_SIZE)
{
	ft_printf("Error: Allocation request exceeds maximum arena size\n");
	return (NULL);
}
*/
void	*ar_alloc(t_arena *arena, size_t bytes)
{
	void	*ptr;
	t_arena	*current;
	t_arena	*new_arena;

	bytes = (bytes + 7) & ~7; // align to 8 bytes -- no need to check
	if (arena->offset + bytes <= arena->size)
	{
		ptr = arena->buffer + arena->offset;
		arena->offset += bytes;
		return (ptr);
	}
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
		arena->offset = 0;
		arena = arena->next;
	}
}
