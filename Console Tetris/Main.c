#include "threads.h"
#include<stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Tetris model.h"

#define INTRVAL 300

bool IsInUse = false;

void Render(Tetris game)
{
	if (IsInUse)
		return;

	IsInUse = true;

	printf("\033[H\033[2J");
	TetrisCell** board = game.board;

	for (int r = 0; r < ROWS; r++)
	{
		printf("|");
		for (int c = 0; c < COLS; c++)
		{

			TetrisCell val = board[r][c];

			if (val == Empty)
				printf("  ");
			else if (val == Moving)
				printf("[]");
			else
				printf("()"); // █
		}
		printf("|");

		if (r == 0)
			printf("      Score: %d", game.Score);

		printf("\n");
	}

	printf("+");
	for (int i = 0; i < COLS * 2; i++)
		printf("-");
	printf("+\n");

	IsInUse = false;
}

int thread_worker(void* arg) {
	Tetris* game = (Tetris*)arg;

	clock_t last = clock();
	while (!(*game).game_over) {
		if (clock() - last >= INTRVAL) {
			last = clock();
			Down(game);
			Render((*game));
		}
	}

	return 0;
}


int main()
{
#ifdef __STDC_NO_THREADS__
	printf("Standard C11 threads are not supported by this compiler.\n");
	return 1;
#endif

	srand(time(NULL));
	thrd_t thread_id;

	Tetris game = new_tetris();
	Render(game);

	if (thrd_create(&thread_id, thread_worker, &game) != thrd_success) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	char ch;
	while (!game.game_over) {
		ch = _getch();
		if (ch == ' ') {
			FullDown(&game);
			Render(game);
		}
		else switch (ch) {
		case 72:
			Rotate(&game);
			Render(game);
			break;
		case 80:
			Down(&game);
			Render(game);
			break;
		case 75:
			Left(&game.board);
			Render(game);
			break;
		case 77:
			Right(&game.board);
			Render(game);
			break;
		}
	}
	printf("Game over");

	int thread_result;
	thrd_join(thread_id, &thread_result);


	return 0;
}