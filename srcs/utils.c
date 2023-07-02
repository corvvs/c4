/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corvvs <corvvs@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/30 12:49:42 by subaru            #+#    #+#             */
/*   Updated: 2023/07/02 14:41:16 by corvvs           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c4.h"

ssize_t	ft_putstr_fd(const char *s, int fd)
{
	size_t	i;

	i = 0;
	while (s[i])
		++i;
	return (write(fd, s, i));
}

int	ft_getchar(void)
{
	unsigned char	c;

	if (read(0, &c, 1) <= 0)
		return (EOF);
	return (c);
}

void	error_exit(const char *s)
{
	ft_putstr_fd(s, STDERR_FILENO);
	exit(EXIT_FAILURE);
}

void	*or_exit(void *alloc)
{
	if (!alloc)
	{
		error_exit("Error\n");
	}
	return (alloc);
}

void	*xcalloc(size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = malloc(n);
	if (!ptr)
	{
		error_exit("malloc() failed\n");
	}
	i = 0;
	while (i < n)
	{
		ptr[i] = 0;
	}
	return (ptr);
}
