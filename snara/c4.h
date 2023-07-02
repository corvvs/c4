#ifndef C4_H
#define C4_H

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>

#define MIN_WIDTH 7
#define MIN_HEIGHT 6
#define MAX_WIDTH 64
#define MAX_HEIGHT 64

#if 1
typedef __uint128_t t_bitmap;
#else
typedef uint64_t t_bitmap;
#endif


typedef struct s_board
{
	size_t HEIGHT; // readonly
	size_t WIDTH;  // readonly
	t_bitmap bottom_mask; // readonly
	t_bitmap board_mask;  // readonly
	t_bitmap current_position; // bitmap of the current_player stones
	t_bitmap mask;			   // bitmap of all the already played spots
} t_board;
typedef unsigned int uint;

typedef struct s_option
{
	size_t HEIGHT; // 盤面の縦サイズ = 行数
	size_t WIDTH;  // 盤面の横サイズ = 列数
	bool on_gui;   // GUI が有効かどうか
} t_option;

typedef struct s_game
{
	t_option*		option;			  // オプション構造体; 探索のことを考えるとポインタにしておく
	unsigned int	game_turn; // ゲームターン数; 0から始まる
	int				current_player;			  // 現手番のプレイヤー; 0 = 人間, 1 = AI
	t_board			board;				  // 現在の盤面
} t_game;

ssize_t	ft_putstr_fd(const char *s, int fd);
int		ft_getchar(void);
bool	ft_parseuint_base(const char *s, unsigned int *np, const char *digits);

#endif
