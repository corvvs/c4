#include "c4.h"

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

bool is_opponent_winning_move(const t_board *self, int col)
{
	return opponent_winning_position(self) & possible(self) & column_mask(self, col);
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

int	put_hand(t_game* game) {
	int			res = -1;
	t_board*	board = &game->board;
	if (game->current_player == 0)
	{
		print_board(board);
		ft_putstr_fd("player's turn:\n", STDOUT_FILENO);
		for (; res < 0;)
		{
			unsigned int	col;
			if (get_col(&col)) {
				res = try_play(game, col - 1);
				if (res >= 0) { break; }
			}
			ft_putstr_fd("accepts only: 1 ~ ", STDOUT_FILENO);
			ft_putnbr_fd(game->board.WIDTH, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
	}
	else if (game->current_player == 1)
	{
		ft_putstr_fd("AI's turn: ", STDOUT_FILENO);
		unsigned int	col = ai_decide(game);
		ft_putnbr_fd(col + 1, STDOUT_FILENO), ft_putstr_fd("\n", STDOUT_FILENO);
		if ((res = try_play(game, col)) < 0)
			ft_putstr_fd("ERROR: AI messed up\n", 2);
	}
	game->game_turn += 1;
	return res;
}

// TODO move to main.c
// TODO implement AI
void	game_loop(t_game* game)
{
	t_board*	board = &game->board;
	for (;;)
	{
		if (put_hand(game) == 0)
		{
			print_board(board);
			ft_putstr_fd((char*[]){"you win!\n", "AI wins!\n"}[game->current_player], STDOUT_FILENO);
			return;
		}
		if (possible(board) == 0)
		{
			print_board(board);
			ft_putstr_fd("draw!\n", STDOUT_FILENO);
			return;
		}
		game->current_player = 1 - game->current_player;
	}
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

	srand(time(NULL));
	t_game	game = {
		.option = &option,
		.board = {
			.HEIGHT = option.HEIGHT,
			.WIDTH = option.WIDTH,
			.board_mask = 0,
			.bottom_mask = 0,
		},
		.current_player = rand() % 2,
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
