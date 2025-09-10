/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 12:15:14 by wshoweky          #+#    #+#             */
/*   Updated: 2025/08/08 12:20:27 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include <stdlib.h>
# include <unistd.h>
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif
# ifndef FDMAX
#  define FDMAX 16
# endif

char	*get_next_line(int fd);
char	*gnl_strjoin(char *s1, char *s2);
size_t	gnl_strlen(char *str);
int		gnl_clean(char *str);
#endif
