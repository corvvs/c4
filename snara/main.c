#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>


#define MIN_WIDTH 7
#define MIN_HEIGHT 6
#define MAX_WIDTH 64
#define MAX_HEIGHT 64

typedef struct s_option {
	size_t height; // 盤面の縦サイズ = 行数
	size_t width;  // 盤面の横サイズ = 列数
	bool   on_gui; // GUI が有効かどうか
} t_option;

typedef struct s_game {
	t_option*          option;         // オプション構造体; 探索のことを考えるとポインタにしておく
	unsigned long long game_turn;      // ゲームターン数; 0から始まる
	int                current_player; // 現手番のプレイヤー; 0 = 人間, 1 = AI
	t_board            board;          // 現在の盤面; 型については未定(1次元で行くか, 配列の配列でいくか)
} t_game;

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
