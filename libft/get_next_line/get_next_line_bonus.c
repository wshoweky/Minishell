/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 12:14:10 by wshoweky          #+#    #+#             */
/*   Updated: 2025/08/08 12:14:12 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*get_next_line(int fd)
{
	static char	buffer[FDMAX][BUFFER_SIZE + 1];
	char		*next_line;
	int			i;

	if (fd < 0 || BUFFER_SIZE < 1 || fd > FDMAX - 1)
		return (0);
	next_line = gnl_strjoin(0, buffer[fd]);
	if (gnl_clean(buffer[fd]) > 0)
		return (next_line);
	i = read(fd, buffer[fd], BUFFER_SIZE);
	if (i < 0)
	{
		free (next_line);
		return (0);
	}
	buffer[fd][i] = '\0';
	while (i > 0)
	{
		next_line = gnl_strjoin(next_line, buffer[fd]);
		if (gnl_clean(buffer[fd]) > 0)
			return (next_line);
		i = read(fd, buffer[fd], BUFFER_SIZE);
	}
	return (next_line);
}
