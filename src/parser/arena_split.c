#include "minishell.h"

static size_t	count_split_words(const char *s, char c)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i] && s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	return (count);
}

static int	fill_split_result(t_arena *arena, char **result, const char *s,
	char c)
{
	size_t	i;
	size_t	j;
	size_t	start;

	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		start = i;
		while (s[i] && s[i] != c)
			i++;
		if (i > start)
		{
			result[j] = ar_substr(arena, s, start, i - start);
			if (!result[j])
				return (0);
			j++;
		}
	}
	result[j] = NULL;
	return (1);
}

char	**ar_split(t_arena *arena, const char *s, char c)
{
	char	**result;
	size_t	count;

	// Normalize NULL input to empty string - eliminates ambiguity
	if (!s)
		s = "";
	count = count_split_words(s, c);
	result = (char **)ar_alloc(arena, (count + 1) * sizeof(char *));
	if (!result)
		return (NULL);  // Only possible cause: allocation failure
	if (!fill_split_result(arena, result, s, c))
		return (NULL);
	return (result);
}
