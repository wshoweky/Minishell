#include "minishell.h"
#include "arena.h"

char	ar_strdup(t_mem_arena *arena, const char *str)
{
	char	*dest;
	size_t	i;

	if (!str)
		return (NULL);
	dest = arena_alloc(arena, ft_strlen(str) + 1);
	if (!dest)
		return (NULL);
	while (str[i])
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
    //    Returns the substring of the given string at the given start
    //    position with the given length (or smaller if the length of the
    //    original string is less than start + length, or length is bigger
    //    than MAXSTRINGLEN).
char	*ar_substr(t_mem_arena *arena, const char *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	n;
	size_t	strlen;
	char	*substr;

	if (!s)
		return (NULL);
	strlen = ft_strlen(s);
	if (start >= strlen)
		return (ar_strdup(arena, "")) //return empty line as it should
	if (len > strlen - start)
		len = strlen - start;	// recorrect the string length
	substr = (char *)arena_alloc(arena, len + 1);
	if (!substr)
		return (NULL);
	i = start;
	n = 0;
	while (s[i] && n < len)
		substr[n++] = s[i++];
	substr[n] = '\0';
	return (substr);
}

char	*ar_strjoin(t_mem_arena *arena, const char *s1, const char *s2)
{
	char	*str;
	size_t	i;
	size_t	n;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (arena_alloc(arena, s2));
	if (!s2)
		return (arena_alloc(arena, s1));
	str = (char *)arena_alloc(arena, ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (NULL);
	i = 0;
	n = 0;
	while (s1[i])
		str[n++] = s1[i++];
	i = 0;
	while (s2[i])
		str[n++] = s2[i++];
	str[n] = '\0';
	return (str);
}

