#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>

#include "Game enums.h"

typedef struct Point
{
	short row, col;
} Point;

Point Add(Point a, Point b) {
	return (Point) { a.row + b.row, a.col + b.col };
}

bool Equals(Point a, Point b) {
	return a.row == b.row && a.col == b.col;
}

typedef struct BoolBoardTuple
{
	TetrisCell** board;
	bool is_can_move;
} BoolBoardTuple;

BoolBoardTuple new_BoolBoardTuple(TetrisCell** board, bool is_can_move) {
	BoolBoardTuple bbt;
	bbt.board = board;
	bbt.is_can_move = is_can_move;
	return bbt;
}


#endif // HELPERS_H
