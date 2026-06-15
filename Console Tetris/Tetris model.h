#ifndef TETRIS_MODEL_H
#define TETRIS_MODEL_H

#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Game enums.h"
#include "Helpers.h"
#include "Offsets.h"

typedef struct Tetris
{
	TetrisCell** board;

	Bounds bound;
	Rotation current_rotation;
	Shape current_shape;

	int Score;
	bool game_over;
} Tetris;


#define ROWS 20    
#define COLS 10

#define SCORE_PER_LINE 100
#define SCORE_PER_TETRIS 800

bool GenerateShape(Tetris* game) {
	Shape new_shape = (Shape)(rand() % SHAPES_COUNT);
	const Point* p = initialazation[new_shape];

	for (int i = 0; i < SHAPES_SIZE; i++)
		if (game->board[p[i].row][p[i].col + COLS / 2 - 2] == Full)
			return false;

	game->bound = DefultBounds();

	for (int i = 0; i < SHAPES_SIZE; i++) {
		int col = p[i].col + COLS / 2 - 2;
		game->board[p[i].row][col] = Moving;

		UpdateBounds(&game->bound, p[i].row, col);
	}

	game->current_rotation = first;
	game->current_shape = new_shape;

	return true;
}


Tetris new_tetris()
{
	Tetris t;
	t.board = (TetrisCell**)malloc(ROWS * sizeof(TetrisCell*));
	for (int i = 0; i < ROWS; i++)
	{
		t.board[i] = (TetrisCell*)malloc(COLS * sizeof(TetrisCell));
		for (int j = 0; j < COLS; j++)
			t.board[i][j] = Empty;
	}
	t.Score = 0;
	t.game_over = false;

	GenerateShape(&t);

	return t;
}

TetrisCell** CopyBoard(TetrisCell** board) {
	TetrisCell** new_board = (TetrisCell**)malloc(ROWS * sizeof(TetrisCell*));
	for (int i = 0; i < ROWS; i++)
	{
		new_board[i] = (TetrisCell*)malloc(COLS * sizeof(TetrisCell));
		for (int j = 0; j < COLS; j++)
			new_board[i][j] = board[i][j];
	}
	return new_board;
}

BoolBoardTuple IsAbleToMove(Tetris* game) {
	TetrisCell** tmp = CopyBoard(game->board);

	for (int row = game->bound.rowMax; row >= game->bound.rowMin; row--) {
		for (int col = game->bound.colMin; col <= game->bound.colMax; col++) {
			if (tmp[row][col] == Moving) {
				if (row == ROWS - 1 || tmp[row + 1][col] == Full)
					return (BoolBoardTuple) { tmp, false };
				else
				{
					tmp[row + 1][col] = Moving;
					tmp[row][col] = Empty;
				}
			}
		}
	}

	return (BoolBoardTuple) { tmp, true };
}


void AfterHiteBottom(Tetris* game) {
	game->Score += DeleteLine(&game->board);

	if (!GenerateShape(game))
		game->game_over = true;
}

bool Down(Tetris* game) {
	BoolBoardTuple bbt = IsAbleToMove(game);
	if (bbt.is_can_move) {
		game->board = bbt.board;
		MoveBoundsDown(&game->bound);
	}
	return bbt.is_can_move;
}

void TimerDown(Tetris* game) {
	BoolBoardTuple bbt = IsAbleToMove(game);

	if (bbt.is_can_move) {
		game->board = bbt.board;
		MoveBoundsDown(&game->bound);
	}
	else
	{
		for (int i = game->bound.rowMin; i <= game->bound.rowMax; i++)
			for (int j = game->bound.colMin; j <= game->bound.colMax; j++)
				if (game->board[i][j] == Moving)
					game->board[i][j] = Full;

		AfterHiteBottom(game);
	}
}

void FullDown(Tetris* game)
{
	BoolBoardTuple bbt;
	do
	{
		bbt = IsAbleToMove(game);

		if (bbt.is_can_move) {
			game->board = bbt.board;
			MoveBoundsDown(&game->bound);
		}
	} while (bbt.is_can_move);

	for (int i = game->bound.rowMin; i <= game->bound.rowMax; i++)
		for (int j = game->bound.colMin; j <= game->bound.colMax; j++)
			if (game->board[i][j] == Moving)
				game->board[i][j] = Full;

	AfterHiteBottom(game);
}

int DeleteLine(TetrisCell*** board) {
	int deleted = 0;
	for (int row = ROWS - 1; row >= 0; row--)
	{
		bool isFull = true;
		for (int col = 0; col < COLS; col++)
		{
			if ((*board)[row][col] != Full)
			{
				isFull = false;
				break;
			}
		}

		if (!isFull)
			continue;

		deleted++;
		for (int r = row; r > 0; r--)
			for (int c = 0; c < COLS; c++)
				(*board)[r][c] = (*board)[r - 1][c];

		for (int c = 0; c < COLS; c++)
			(*board)[0][c] = Empty;

		row++;
	}

	return deleted == SHAPES_SIZE ? SCORE_PER_TETRIS : deleted * SCORE_PER_LINE;
}

bool Left(Tetris* game)
{
	if (game->bound.colMin == 0)
		return false;

	bool is_left = true;
	TetrisCell** tmp = CopyBoard(game->board);

	for (int i = game->bound.rowMin; i <= game->bound.rowMax; i++)
	{
		for (int j = game->bound.colMin; j <= game->bound.colMax; j++)
		{
			if (tmp[i][j] == Moving)
			{
				if (j == 0 || tmp[i][j - 1] == Full)
				{
					is_left = false;
					break;
				}
				else
				{
					tmp[i][j - 1] = Moving;
					tmp[i][j] = Empty;
				}
			}
		}
	}

	if (is_left) {
		game->board = tmp;
		MoveBoundsLeft(&game->bound);
	}

	return is_left;
}

bool Right(Tetris* game)
{
	if (game->bound.colMax == ROWS - 1)
		return false;

	bool is_right = true;
	TetrisCell** tmp = CopyBoard(game->board);

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = COLS - 1; j >= 0; j--)
		{
			if (tmp[i][j] == Moving)
			{
				if (j == COLS - 1 || tmp[i][j + 1] == Full)
				{
					is_right = false;
					break;
				}
				else
				{
					tmp[i][j + 1] = Moving;
					tmp[i][j] = Empty;
				}
			}
		}
	}

	if (is_right) {
		game->board = tmp;
		MoveBoundsRight(&game->bound);
	}

	return is_right;
}

void RotateShape(Tetris* game, const Point* indexes)
{
	Point* original = malloc(sizeof(Point) * SHAPES_SIZE);
	int idx = 0;
	for (int row = 0; row < ROWS; row++)
		for (int col = 0; col < COLS; col++)
			if (game->board[row][col] == Moving)
				original[idx++] = (Point){ row, col };

	if (idx != SHAPES_SIZE)
		return;

	Point* targets = malloc(sizeof(Point) * SHAPES_SIZE);

	for (int i = 0; i < SHAPES_SIZE; i++)
	{
		Point tgt = Add(original[i], indexes[i]);

		if (tgt.col < 0 || tgt.col >= COLS || tgt.row < 0 || tgt.row >= ROWS)
			return;

		targets[i] = tgt;
	}

	for (int i = 0; i < SHAPES_SIZE; i++)
	{
		Point t = targets[i];

		for (int j = i + 1; j < SHAPES_SIZE; j++)
			if (Equals(t, targets[j]))
				return;

		bool isOriginal = false;
		for (int j = 0; j < SHAPES_SIZE; j++)
			if (Equals(t, original[j]))
				isOriginal = true;

		if (!isOriginal && game->board[t.row][t.col] != Empty)
			return;
	}

	TetrisCell** tmp = CopyBoard(game->board);
	for (int i = 0; i < SHAPES_SIZE; i++)
		tmp[original[i].row][original[i].col] = Empty;

	game->bound = DefultBounds();

	for (int i = 0; i < SHAPES_SIZE; i++) {
		tmp[targets[i].row][targets[i].col] = Moving;

		UpdateBounds(&game->bound, targets[i].row, targets[i].col);
	}


	game->board = tmp;
	game->current_rotation = (Rotation)(((int)game->current_rotation + 1) % ROTATIONS_COUNT);
}


bool Rotate(Tetris* game)
{
	if (game->current_shape == O)
		return false;

	if (game->current_shape == I) // special range
	{
		if (
			game->current_rotation == first && 0 < game->bound.rowMin && game->bound.rowMax < ROWS - 2 ||
			game->current_rotation == second && 1 < game->bound.colMin && game->bound.colMax < COLS - 1 ||
			game->current_rotation == third && 1 < game->bound.rowMin && game->bound.rowMax < ROWS - 1 ||
			game->current_rotation == fourth && 0 < game->bound.colMin && game->bound.colMax < COLS - 2) {
			RotateShape(game, rotation[game->current_shape][game->current_rotation]);
			return true;
		}
	}
	else if (
		game->current_rotation == first && game->bound.rowMax < ROWS - 1 ||
		game->current_rotation == second && 0 < game->bound.colMin ||
		game->current_rotation == third && 0 < game->bound.rowMin ||
		game->current_rotation == fourth && game->bound.colMax < COLS - 1) {
		RotateShape(game, rotation[game->current_shape][game->current_rotation]);
		return true;
	}

	return false;
}

#endif // TETRIS_MODEL_H
