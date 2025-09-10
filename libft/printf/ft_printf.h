/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:51:21 by wshoweky          #+#    #+#             */
/*   Updated: 2025/05/19 11:52:49 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stdarg.h>
# include <unistd.h>

int	ft_printf(const char *format, ...);

int	check_format(const char *format, va_list ap, int *check);

int	is_char(int c, int *check);

int	is_str(char *str, int *check);

int	is_signed(int n, int *check);

int	is_unsigned(unsigned long long n, int base, int cap, int *check);

int	is_ptr(void *ptr, int *check);

#endif
