#include "minishell.h"

char	*ar_strdup(t_arena *arena, const char *str)
{
	char	*dest;
	size_t	i;

	// Normalize NULL input to empty string - eliminates ambiguity
	if (!str)
		str = ""; // points to null terminator - empty string ['\0'] to prevent segmentation fault
	i = 0;
	dest = ar_alloc(arena, ft_strlen(str) + 1);
	if (!dest)
		return (NULL);  // Only possible cause: allocation failure
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
char	*ar_substr(t_arena *arena, const char *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	n;
	size_t	strlen;
	char	*substr;

	// Normalize NULL input to empty string - eliminates ambiguity
	if (!s)
		s = "";
	strlen = ft_strlen(s);
	if (start >= strlen)
		return (ar_strdup(arena, "")); //return empty line as it should
	if (len > strlen - start)
		len = strlen - start;	// recorrect the string length
	substr = (char *)ar_alloc(arena, len + 1);
	if (!substr)
		return (NULL);  // Only possible cause: allocation failure
	i = start;
	n = 0;
	while (s[i] && n < len)
		substr[n++] = s[i++];
	substr[n] = '\0';
	return (substr);
}

char	*ar_strjoin(t_arena *arena, const char *s1, const char *s2)
{
	char	*str;
	size_t	i;
	size_t	n;

	// Normalize NULL inputs to empty strings - eliminates ambiguity
	if (!s1)
		s1 = "";
	if (!s2)
		s2 = "";
	str = (char *)ar_alloc(arena, ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (NULL);  // Only possible cause: allocation failure
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


char	*ar_add_char_to_str(t_arena *arena, char *s, char c)
{
	size_t	len;
	char	*result;

	if (!s)
		len = 0;
	else
		len = ft_strlen(s);
	result = (char *)ar_alloc(arena, len + 2);
	if (!result)
		return (NULL);
	if (s)
		ft_memcpy(result, s, len);
	result[len] = c;
	result[len + 1] = '\0';
	return (result);
}

