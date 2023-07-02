#include "c4.h"

bool can_play(const t_board *self, int col);
bool is_winning_move(const t_board *self, int col);
bool is_opponent_winning_move(const t_board *self, int col);

int	ai_decide(const t_game* game)
{
	const t_board*	board = &game->board;

	for (uint i = 0; i < board->WIDTH; i++)
	{
		if (can_play(board, i) && is_winning_move(board, i)) // ai can win
			return i;
	}
	for (uint i = 0; i < board->WIDTH; i++)
	{
		if (can_play(board, i) && is_opponent_winning_move(board, i)) // opponent would win
			return i;
	}

	// column exploration order, starting with center columns
	// example for WIDTH=7: columnOrder = {3, 4, 2, 5, 1, 6, 0}
	for (int i = 0; (size_t)i < board->WIDTH; i++)
	{
		int col = board->WIDTH / 2 + (1 - 2 * (i % 2)) * (i + 1) / 2;
		if (can_play(board, col))
			return col;
	}
	return 0; // hopefully never
}

