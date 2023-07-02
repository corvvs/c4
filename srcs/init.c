#include "c4.h"

bool	parse_args(int argc, char *argv[], t_option *option)
{
	unsigned int	width = 0;
	unsigned int	height = 0;
	bool			on_gui = false;

	if (argc < 3 || 4 < argc)
	{
		ft_putstr_fd("usage: ./connect4 width height\n", 2);
		return false;
	}
	if (!ft_parseuint_base(argv[1], &width, "0123456789")
		|| !(MIN_WIDTH <= width && width <= MAX_WIDTH))
	{
		ft_putstr_fd("invalid width\n", 2);
		return false;
	}
	if (!ft_parseuint_base(argv[1], &height, "0123456789")
		|| !(MIN_HEIGHT <= height && height <= MAX_HEIGHT))
	{
		ft_putstr_fd("invalid height\n", 2);
		return false;
	}
	if (argc == 4) //TODO gui option
	{
		on_gui = true;
	}
	*option = (t_option){height, width, on_gui};
	return true;
}

bool	init_board(t_game* game, const t_option* option)
{
	if (option->WIDTH * (option->HEIGHT+1) > sizeof(t_bitmap) * 8)
	{
		return false;
	}
	for (size_t i = 0; i < option->WIDTH; i++)
	{
		game->board.bottom_mask |= ((t_bitmap)1 << (i * (option->HEIGHT + 1)));
	}
	game->board.board_mask = game->board.bottom_mask * (((t_bitmap)1 << option->HEIGHT) - 1);
	return true;
}
