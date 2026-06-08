#ifndef TETRIS_MODEL_H
#define TETRIS_MODEL_H

#include <stdbool.h>
#include <stdlib.h>

#include "Game enums.h"
#include "Helpers.h"
#include "Offsets.h"

typedef struct Tetris
{
	TetrisCell** board;
	int Score;

	Rotation current_rotation;
	Shape current_shape;

	bool game_over;
} Tetris;


#define ROWS 20
#define COLS 10

#define SCORE_PER_LINE 100
#define SCORE_PER_TETRIS 800

bool GenerateShape(Tetris* game) {
	Shape new_shape = (Shape)(rand() % SHAPES_COUNT);
	Point *p = initialazation[new_shape];

	for (int i = 0; i < SHAPES_SIZE; i++)
		if ((*game).board[p[i].row][p[i].col] == Full)
			return false;


	for (int i = 0; i < SHAPES_SIZE; i++)
		(*game).board[p[i].row][p[i].col] = Moving;

	(*game).current_rotation = first;
	(*game).current_shape = new_shape;

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

BoolBoardTuple IsAbleToMove(TetrisCell** board) {
	TetrisCell** tmp = CopyBoard(board);

	for (int row = ROWS - 1; row >= 0; row--) {
		for (int col = 0; col < COLS; col++) {
			if (tmp[row][col] == Moving) {
				if (row == ROWS - 1 || tmp[row + 1][col] == Full) {
					return new_BoolBoardTuple(tmp, false);
				}
				else
				{
					tmp[row + 1][col] = Moving;
					tmp[row][col] = Empty;
				}
			}
		}
	}

	return new_BoolBoardTuple(tmp, true);
}


void AfterHiteBottom(Tetris* game) {
	(*game).Score += DeleteLine(&(*game).board);

	if (!GenerateShape(game))
		(*game).game_over = true;
}

void Down(Tetris* game) {
	BoolBoardTuple bbt = IsAbleToMove((*game).board);

	if (bbt.is_can_move)
		(*game).board = bbt.board;
	else
	{
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++)
				if ((*game).board[i][j] == Moving)
					(*game).board[i][j] = Full;

		AfterHiteBottom(game);
	}
}

void FullDown(Tetris* game)
{
	BoolBoardTuple bbt;
	do
	{
		bbt = IsAbleToMove((*game).board);

		if (bbt.is_can_move)
			(*game).board = bbt.board;
	} while (bbt.is_can_move);

	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if ((*game).board[i][j] == Moving)
				(*game).board[i][j] = Full;

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
		{
			for (int c = 0; c < COLS; c++)
			{
				(*board)[r][c] = (*board)[r - 1][c];
			}
		}

		for (int c = 0; c < COLS; c++)
			(*board)[0][c] = Empty;

		row++;
	}

	return deleted == SHAPES_SIZE ? SCORE_PER_TETRIS : deleted * SCORE_PER_LINE;
}

void Left(TetrisCell*** board)
{
	bool is_left = true;
	TetrisCell** tmp = CopyBoard(*board);

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
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

	if (is_left)
		(*board) = tmp;

}

void Right(TetrisCell*** board)
{
	bool is_right = true;
	TetrisCell** tmp = CopyBoard(*board);

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

	if (is_right)
		(*board) = tmp;

}


void RotateShape(Tetris* game, Point* indexes)
{
	Point* original = malloc(sizeof(Point) * SHAPES_SIZE);
	int idx = 0;
	for (int row = 0; row < ROWS; row++)
		for (int col = 0; col < COLS; col++)
			if ((*game).board[row][col] == Moving)
				original[idx++] = new_Point(row, col);

	if (idx != 4)
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

		for (int j = i + 1; j < 4; j++)
			if (Equals(t, targets[j]))
				return;

		bool isOriginal = false;
		for (int j = 0; j < SHAPES_SIZE; j++)
			if (Equals(t, original[j]))
				isOriginal = true;

		if (!isOriginal && (*game).board[t.row][t.col] != Empty)
			return;
	}

	TetrisCell** tmp = CopyBoard((*game).board);
	for (int i = 0; i < SHAPES_SIZE; i++)
		tmp[original[i].row][original[i].col] = Empty;

	for (int i = 0; i < SHAPES_SIZE; i++)
		tmp[targets[i].row][targets[i].col] = Moving;

	(*game).board = tmp;
	(*game).current_rotation = (Rotation)(((int)(*game).current_rotation + 1) % 4);
}


void Rotate(Tetris* game)
{
	if ((*game).current_shape == O)
		return;

	int idx = 0;
	int rowMin = -1, rowMax = -1, colMin = -1, colMax = -1;

	for (int i = 0; i < ROWS && idx < SHAPES_SIZE; i++)
	{
		for (int j = 0; j < COLS && idx < SHAPES_SIZE; j++)
		{
			if ((*game).board[i][j] == Moving)
			{
				if (i < rowMin || rowMin == -1)
					rowMin = i;

				if (rowMax < i || rowMax == -1)
					rowMax = i;

				if (j < colMin || colMin == -1)
					colMin = j;

				if (colMax < j || colMax == -1)
					colMax = j;

				idx++;
			}
		}
	}


	if ((*game).current_shape == I) // special range
	{
		if (
			(*game).current_rotation == first && rowMin > 0 && rowMax < 18 ||
			(*game).current_rotation == second && colMin > 1 && colMax < 9 ||
			(*game).current_rotation == third && rowMin > 1 && rowMax < 19 ||
			(*game).current_rotation == fourth && colMin > 0 && colMax < 8
			)
			RotateShape(game, rotation[(*game).current_shape][(*game).current_rotation]);
	}
	else if (
		(*game).current_rotation == first && rowMax < 19 ||
		(*game).current_rotation == second && 0 < colMin ||
		(*game).current_rotation == third && 0 < rowMin ||
		(*game).current_rotation == fourth && colMax < 9)
		RotateShape(game, rotation[(*game).current_shape][(*game).current_rotation]);

	printf("%d", rowMax);
}




#endif // TETRIS_MODEL_H
