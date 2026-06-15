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


typedef struct Bounds {
	int rowMax, rowMin, colMax, colMin;
} Bounds;

Bounds DefultBounds() {
	return (Bounds) { -1, -1, -1, -1 };
}

void UpdateBounds(Bounds* bound, int row, int col) {
	if (row < bound->rowMin || bound->rowMin == -1)
		bound->rowMin = row;

	if (bound->rowMax < row || bound->rowMax == -1)
		bound->rowMax = row;

	if (col < bound->colMin || bound->colMin == -1)
		bound->colMin = col;

	if (bound->colMax < col || bound->colMax == -1)
		bound->colMax = col;
}

void MoveBoundsDown(Bounds* bound) {
	bound->rowMax++;
	bound->rowMin++;
}

void MoveBoundsLeft(Bounds* bound) {
	bound->colMax--;
	bound->colMin--;
}

void MoveBoundsRight(Bounds* bound) {
	bound->colMax++;
	bound->colMin++;
}


#endif // HELPERS_H
