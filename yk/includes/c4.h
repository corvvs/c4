#ifndef C4_H
#define C4_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WIDTH  64
#define MAX_HEIGHT 64

typedef struct s_option {
	int		width;
	int		height;
	bool	use_gui;
} t_option;

typedef int (t_grid)[MAX_HEIGHT][MAX_WIDTH];

typedef struct s_game {
	t_option*	option;
	t_grid		grid; // 0: empty, 1: player 1, 2: player 2
	int			turn;
	bool		is_manual[2]; // 0: player 1, 1: player 2
} t_game;

#endif
