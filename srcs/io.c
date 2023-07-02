#include "c4.h"

#define BUF_SIZE 1024

int	get_col(unsigned int* np) {
	char buf[BUF_SIZE];

	int rs = read(STDIN_FILENO, buf, BUF_SIZE);
	if (rs <= 0) {
		return -1;
	}
	char *nl = ft_memchr(buf, '\n', rs);
	if (nl == NULL)
		return -1;
	*nl = '\0';
	return ft_parseuint_base(buf, np, "0123456789");
}

// return board[x,y]
// 0 if empty
// 1 if belongs to current_player
// 2 if belongs to opponent
// -1 on error
static int	board_at(const t_board* self, int x, int y)
{
	if (!(0 <= x && (size_t)x < self->WIDTH && 0 <= y && (size_t)y < self->HEIGHT))
		return -1;

	t_bitmap	pos = ((t_bitmap)1) << ((x * (self->HEIGHT + 1)) + (y));
	if ((pos & self->mask) == 0)
		return 0;
	if ((pos & self->current_position) != 0)
		return 1;
	if ((pos & self->current_position) == 0)
		return 2;
	return -1;
}


void	print_board(const t_board* self)
{
	ft_putstr_fd("╭", STDOUT_FILENO);
	for (size_t x = 0; x < self->WIDTH; ++x)
	{
		ft_putstr_fd("─", STDOUT_FILENO);
		ft_putstr_fd(x + 1 < self->WIDTH ? "┬" :  "╮", STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);

	for (int y = self->HEIGHT - 1; y >= 0; --y)
	{
		ft_putstr_fd("│", STDOUT_FILENO);
		for (size_t x = 0; x < self->WIDTH; ++x)
		{
			write(STDOUT_FILENO, &"? ox"[1+board_at(self, x, y)], 1);
			ft_putstr_fd("│", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);

		ft_putstr_fd(1 <= y ? "├" : "╰", STDOUT_FILENO);
		for (size_t x = 0; x < self->WIDTH; ++x)
		{
			ft_putstr_fd("─", STDOUT_FILENO);
			if (1 <= y) {
				ft_putstr_fd(x + 1 < self->WIDTH ? "┼" :  "┤", STDOUT_FILENO);
			} else {
				ft_putstr_fd(x + 1 < self->WIDTH ? "┴" :  "╯", STDOUT_FILENO);
			}
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
	}

	for (size_t x = 1; x <= self->WIDTH; ++x)
	{
		ft_putstr_fd(" ", STDOUT_FILENO);
		ft_putnbr_fd(x, STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);
}
