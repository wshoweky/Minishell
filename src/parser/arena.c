#include "arena.h"
#include <stddef.h>
#include <stdlib.h>

t_mem_arena	*arena_init()
{
	t_mem_arena	*arena;

	arena = malloc(sizeof(t_mem_arena));
	if (!arena)
		return (NULL);
	arena->size = A_SIZE;
	arena->buffer = malloc(arena->size);
	if (!arena->buffer)
	{
		free(arena);
		return (NULL);
	}
	arena->offset = 0;
	return (arena);
}

void	*arena_alloc(t_mem_arena *arena, size_t bytes)
{
	void	*ptr;

	if (arena->offset + bytes > arena->size)
		return (NULL);
	ptr = arena->buffer + arena->offset;
	arena->offset += bytes;
	return (ptr);
}

void	free_arena(t_mem_arena *arena)
{
	t_mem_arena	*tmp;

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

void	arena_reset(t_mem_arena *arena)
{
	while (arena)
	{
		arena->offset = 0;
		arena = arena->next;
	}
}
