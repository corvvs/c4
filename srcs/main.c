#include "c4.h"

bool	ft_parseuint_base(const char *s, unsigned int *np, const char *digits);
ssize_t	ft_putstr_fd(const char *s, int fd);

bool	init_board(size_t x, size_t y);

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
	if (!init_board(width, height))
	{
		ft_putstr_fd("board too large\n", 2);
		return false;
	}
	if (argc == 4) //TODO gui option
	{
		on_gui = true;
	}
	*option = (t_option){height, width, on_gui};
	return true;
}

void	game_loop();

int	main(int argc, char *argv[])
{
	t_option	option = {};
	if (!parse_args(argc, argv, &option))
		return 1;
	t_game		g = {&option, 0, 0, {}};
}
