/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:50:48 by wshoweky          #+#    #+#             */
/*   Updated: 2025/04/30 12:18:24 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t		start;
	size_t		end;
	size_t		len;
	char		*trimmed;

	if (!s1 || !set)
		return (0);
	if (!*s1)
		return (ft_strdup(s1));
	start = 0;
	len = ft_strlen(s1);
	end = len;
	while (start < len && ft_strchr(set, s1[start]))
		start++;
	while (end > start && ft_strchr(set, s1[end - 1]))
		end--;
	if (start > end)
		return (ft_strdup(""));
	trimmed = malloc(end - start + 1);
	if (!trimmed)
		return (0);
	ft_strlcpy(trimmed, s1 + start, end - start + 1);
	return (trimmed);
}
