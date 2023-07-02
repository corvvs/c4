#include "c4.h"

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

// unused
bool alignment(const t_board* self, t_bitmap pos)
{
	t_bitmap m;
	size_t HEIGHT = self->HEIGHT;
	// horizontal
	m = pos & (pos >> (HEIGHT+1));
	if(m & (m >> (2*(HEIGHT+1))))
		return true;

	// diagonal 1
	m = pos & (pos >> HEIGHT);
	if(m & (m >> (2*HEIGHT)))
		return true;

	// diagonal 2
	m = pos & (pos >> (HEIGHT+2));
	if(m & (m >> (2*(HEIGHT+2))))
		return true;

	// vertical;
	m = pos & (pos >> 1);
	if(m & (m >> 2))
		return true;

	return false;
}

/**
 * @parmam position, a bitmap of the player to evaluate the winning pos
 * @param mask, a mask of the already played spots
 *
 * @return a bitmap of all the winning free spots making an alignment
 */
static t_bitmap compute_winning_position(const t_board *self, t_bitmap position, t_bitmap mask)
{
	size_t HEIGHT = self->HEIGHT;
	// vertical;
	t_bitmap r = (position << 1) & (position << 2) & (position << 3);
	t_bitmap p;

	//horizontal
	p = (position << (HEIGHT + 1)) & (position << 2 * (HEIGHT + 1));
	r |= p & (position << 3 * (HEIGHT + 1));
	r |= p & (position >> (HEIGHT + 1));
	p = (position >> (HEIGHT + 1)) & (position >> 2 * (HEIGHT + 1));
	r |= p & (position << (HEIGHT + 1));
	r |= p & (position >> 3 * (HEIGHT + 1));

	//diagonal 1
	p = (position << HEIGHT) & (position << 2 * HEIGHT);
	r |= p & (position << 3 * HEIGHT);
	r |= p & (position >> HEIGHT);
	p = (position >> HEIGHT) & (position >> 2 * HEIGHT);
	r |= p & (position << HEIGHT);
	r |= p & (position >> 3 * HEIGHT);

	//diagonal 2
	p = (position << (HEIGHT + 2)) & (position << 2 * (HEIGHT + 2));
	r |= p & (position << 3 * (HEIGHT + 2));
	r |= p & (position >> (HEIGHT + 2));
	p = (position >> (HEIGHT + 2)) & (position >> 2 * (HEIGHT + 2));
	r |= p & (position << (HEIGHT + 2));
	r |= p & (position >> 3 * (HEIGHT + 2));

	return r & (self->board_mask ^ mask);
}


// return a bitmask containg a single 1 corresponding to the top cell of a given column
static t_bitmap	top_mask_col(const t_board* self, int col)
{
	return ((t_bitmap)1) << ((self->HEIGHT - 1) + col * (self->HEIGHT + 1));
}

// return a bitmask containg a single 1 corresponding to the bottom cell of a given column
static t_bitmap	bottom_mask_col(const t_board* self, int col)
{
	return ((t_bitmap)1) << (col * (self->HEIGHT + 1));
}

// return a bitmask 1 on all the cells of a given column
static t_bitmap	column_mask(const t_board* self, int col)
{
	return (((t_bitmap)1 << self->HEIGHT) - 1) << (col * (self->HEIGHT + 1));
}

// return a bitmask 1 on all the cells of a given row
t_bitmap	row_mask(const t_board* self, int row)
{
	return (self->bottom_mask) << (row);
}

/**
 * Plays a possible move given by its bitmap representation
 *
 * @param move: a possible move given by its bitmap representation
 *        only one bit of the bitmap should be set to 1
 *        the move should be a valid possible move for the current player
 */
static void play(t_board *self, t_bitmap move)
{
	self->current_position ^= self->mask;
	self->mask |= move;
	//self->moves++;
}

// plays a playable column
static void play_col(t_board *self, int col) {
	play(self, (self->mask + bottom_mask_col(self, col)) & column_mask(self, col));
}

// whether a column is playable
bool can_play(const t_board *self, int col)
{
	return (self->mask & top_mask_col(self, col)) == 0;
}

// Return a bitmask of the possible winning positions for the current player
t_bitmap winning_position(const t_board *self)
{
	return compute_winning_position(self, self->current_position, self->mask);
}

// Return a bitmask of the possible winning positions for the opponent
t_bitmap opponent_winning_position(const t_board *self)
{
	return compute_winning_position(self, self->current_position ^ self->mask, self->mask);
}

// Bitmap of the next possible valid moves for the current player including losing moves.
static t_bitmap possible(const t_board *self)
{
	return (self->mask + self->bottom_mask) & self->board_mask;
}

// Indicates whether the current player wins by playing a given column
bool is_winning_move(const t_board *self, int col)
{
	return winning_position(self) & possible(self) & column_mask(self, col);
}


// return true if current player can win next move
bool can_win_next(const t_board *self)
{
	return winning_position(self) & possible(self);
}

// return a compact representation of a position on WIDTH*(HEIGHT+1) bits.
t_bitmap key(const t_board *self)
{
	return self->current_position + self->mask;
}

/*
 * Plays a sequence of successive played columns, mainly used to initilize a board.
 * @param seq: a sequence of digits corresponding to the 1-based index of the column played.
 *
 * @return number of played moves. Processing will stop at first invalid move that can be:
 *           - invalid character (non digit, or digit >= WIDTH)
 *           - playing a colum the is already full
 *           - playing a column that makes an alignment (we only solve non).
 *         Caller can check if the move sequence was valid by comparing the number of
 *         processed moves to the length of the sequence.
 */
size_t play_seq(t_board *self, const char *seq)
{
	size_t i;
	for (i = 0; seq[i]; i++)
	{
		int col = seq[i] - '1';
		if (col < 0 || (size_t)col >= self->WIDTH || !can_play(self, col))
			return i; // invalid move
		if (is_winning_move(self, col))
			return i; // invalid?
		play_col(self, col);
	}
	return i;
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

#if 1

#include <stdio.h>
//TODO get rid of printf()
void	print_board(const t_board* self)
{
	for (int y = self->HEIGHT-1; y >= 0; --y)
	{
		for (size_t x = 0; x < self->WIDTH; ++x)
		{
			write(STDOUT_FILENO, &"?_ox"[1+board_at(self, x, y)], 1);
			ft_putstr_fd(" ", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
	}
	for (size_t x = 0; x < self->WIDTH; ++x)
	{
		ft_putstr_fd("==", STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);
	for (size_t x = 1; x <= self->WIDTH; ++x)
	{
		ft_putnbr_fd(x, STDOUT_FILENO);
		ft_putstr_fd(" ", STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);
}

#endif

#if 1

int try_play(t_game* game, int col)
{
	t_board*	self = &game->board;
	if (col < 0 || (size_t)col >= game->option->WIDTH || !can_play(self, col))
	{
		return -1; // invalid move
	}
	if (is_winning_move(self, col))
	{
		play_col(self, col);
		return 0; // game set
	}
	play_col(self, col);
	return 1; // continue
}

// TODO move to main.c
// TODO implement AI
void	game_loop(t_game* game)
{
	t_board*	board = &game->board;
	print_board(board);
	for (;;)
	{
		if (possible(board) == 0)
		{
			ft_putstr_fd("draw!\n", STDOUT_FILENO);
			return;
		}

		ft_putstr_fd("player ", STDOUT_FILENO);
		ft_putnbr_fd(game->game_turn%2, STDOUT_FILENO);
		ft_putstr_fd(" >\n", STDOUT_FILENO);
		int	res = -1;
		for (; res < 0;)
		{
			unsigned int	col;
			if (!get_col(&col)) { continue; }
			res = try_play(game, col - 1);
		}

		game->game_turn += 1;
		print_board(board);
		if (res == 0)
		{
			ft_putstr_fd("player ", STDOUT_FILENO);
			ft_putnbr_fd(game->game_turn % 2, STDOUT_FILENO);
			ft_putstr_fd(" wins!\n", STDOUT_FILENO);
			return;
		}
	}
}

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

// usage: ./a.out "12344321"
int	main(int ac, char *av[])
{
	t_option	option = {};
	if (!parse_args(ac, av, &option))
		return 1;
	ft_putstr_fd("h: ", STDOUT_FILENO);
	ft_putnbr_fd(option.HEIGHT, STDOUT_FILENO);
	ft_putstr_fd(", w: ", STDOUT_FILENO);
	ft_putnbr_fd(option.WIDTH, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	t_game	game = {
		.option = &option,
		.board = {
			.HEIGHT = option.HEIGHT,
			.WIDTH = option.WIDTH,
			.board_mask = 0,
			.bottom_mask = 0,
		},
	};

	if (!init_board(&game, &option)) {
		ft_putstr_fd("too large size!!\n", STDOUT_FILENO);
		return 1;
	}

	// printf("bitsize[%zu]\nbottom[%llx]\n board[%llx]\n", sizeof(t_bitmap)*8, bottom_mask, board_mask);

	game_loop(&game);
	// if (ac >= 2)
	// {
	// 	t_board	board = {
	// 		.HEIGHT = 6,
	// 		.WIDTH = 7,
	// 		.board_mask = 0,
	// 		.bottom_mask = 0,
	// 	};
	// 	print_board(&board);
	// 	printf("\nprocessed %zu turns\n", play_seq(&board, av[ac-1]));
	// 	print_board(&board);
	// 	return 0;
	// }
}

#endif
