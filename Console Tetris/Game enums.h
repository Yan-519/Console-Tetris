#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H

typedef enum TetrisCell
{
	Empty,
	Full,
	Moving
} TetrisCell;

typedef enum Rotation
{
	first,
	second,
	third,
	fourth
} Rotation;

#define SHAPES_COUNT 7
#define SHAPES_SIZE 4

typedef enum Shape
{
	I, T, S, Z, J, L, O
} Shape;

#endif // GAME_ENUMS_H
