/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parseint.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corvvs <corvvs@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 15:32:03 by subaru            #+#    #+#             */
/*   Updated: 2023/07/02 14:41:05 by corvvs           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c4.h"

#define K_MAX_NUM INT_MAX

typedef uint64_t	t_ull;

static int	ft_isspace(int c)
{
	return (c == ' ' || ('\t' <= c && c <= '\r'));
}

ssize_t	ft_strfind(const char *s, int c)
{
	size_t	i;

	i = 0;
	while (s[i] != c && s[i])
	{
		++i;
	}
	if (s[i] == (char)c)
		return (i);
	return (-1);
}

static bool	calc_overflow(t_ull *n, int c, int base)
{
	const t_ull	limit = (t_ull)K_MAX_NUM + (base < 0);

	if (c < 0)
	{
		errno = EINVAL;
		return (false);
	}
	if (base < 0)
		base = -base;
	if (*n > limit / base || (*n == limit / base && (t_ull)c > limit % base))
	{
		*n = limit;
		errno = ERANGE;
		return (false);
	}
	else
	{
		*n = *n * base + c;
		return (true);
	}
}

bool	ft_parseint_base(const char *s, int *np, const char *digits)
{
	const int	b = ft_strfind(digits, '\0');
	int			sign;
	t_ull		n;
	size_t		i;

	if (b < 2)
		return (false);
	n = 0;
	sign = 1;
	while (ft_isspace(*s))
		++s;
	if (*s == '+' || *s == '-')
		sign = -(*s++ - ',');
	i = 0;
	while (s[i])
	{
		if (!calc_overflow(&n, ft_strfind(digits, s[i++]), b * sign))
			return (false);
	}
	if (i < 1)
		return (false);
	*np = n * sign;
	return (true);
}

bool	ft_parseuint_base(const char *s, unsigned int *np, const char *digits)
{
	const int	b = ft_strfind(digits, '\0');
	t_ull		n;
	size_t		i;

	if (b < 2)
		return (false);
	n = 0;
	while (ft_isspace(*s))
		++s;
	i = 0;
	while (s[i])
	{
		if (!calc_overflow(&n, ft_strfind(digits, s[i++]), b))
			return (false);
	}
	if (i < 1)
		return (false);
	*np = n;
	return (true);
}

#if 0
//# include <stdio.h>

int	main(int ac, char *av[])
{
	printf("%d %d\n", ft_parseint_base(av[1], &ac, "0123456789"), ac);
}

#endif
