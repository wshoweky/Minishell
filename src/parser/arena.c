#include "minishell.h"

t_arena	*ar_init()
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

	// Align bytes to ensure proper memory alignment
	//This line rounds up the requested allocation size to the next multiple of 8 bytes.
	bytes = (bytes + 7) & ~7;

	// Check if allocation exceeds maximum arena size
	if (bytes > AR_SIZE)
	{
		ft_printf("Error: Allocation request of %zu bytes exceeds maximum arena size\n", bytes);
		return (NULL);
	}

	// Try to allocate in the current arena
	if (arena->offset + bytes <= arena->size)
	{
		ptr = arena->buffer + arena->offset;
		arena->offset += bytes;
		return (ptr);
	}

	// If we get here, we need to find an arena with enough space or create a new one
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

	// No existing arena has enough space, create a new one
	new_arena = ar_init();
	if (!new_arena)
		return (NULL);
	
	current->next = new_arena;
	
	// Allocate from the new arena
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
