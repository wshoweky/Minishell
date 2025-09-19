#ifndef	ARENA_H
#define	ARENA_H

#include "minishell.h"

#define A_SIZE	(2 * 1024 * 1024) //2MB

typedef struct	s_mem_arena
{
	void	*buffer;	//pointer to start of arena
	size_t	size;		//the size of the arena
	size_t	offset;		//tracking the start of free space
	struct	s_mem_arena	*next;	//pointer to the next arena
}	t_mem_arena;

t_mem_arena	*arena_init();
void	*arena_alloc(t_mem_arena *arena, size_t bytes);
void	free_arena(t_mem_arena *arena);
//TODO
//ar_strdup
//ar_substr
//ar_strjoin

#endif
