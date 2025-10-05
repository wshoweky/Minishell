#ifndef	ARENA_H
#define	ARENA_H

//# include "minishell.h" can't be included here due to circular dependency
# define AR_SIZE	(2 * 1024 * 1024) //2MB

typedef struct	s_arena
{
	void	*buffer;	//pointer to start of arena
	size_t	size;		//the size of the arena
	size_t	offset;		//tracking the start of free space
	struct	s_arena	*next;	//pointer to the next arena
}	t_arena;

// Arena core functions
t_arena	*ar_init();
void	*ar_alloc(t_arena *arena, size_t bytes);
int		normally_fit_in_arena(t_arena *arena, void **ptr, size_t *bytes);
void	free_arena(t_arena *arena);
void	ar_reset(t_arena *arena);

// Arena utility functions
char	*ar_strdup(t_arena *arena, const char *str);
char	*ar_substr(t_arena *arena, const char *s, unsigned int start, size_t len);
char	*ar_strjoin(t_arena *arena, const char *s1, const char *s2);
char	**ar_split(t_arena *arena, const char *s, char c);
char	*ar_add_char_to_str(t_arena *arena, char *s, char c);

#endif
