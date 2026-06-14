#include "threads.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Game enums.h"
#include "Tetris model.h"

#define INTERVAL 300
#define MIN_INTERVAL 15

#define SCORE_INFLUENCE_ON_INTERVAL 35

bool IsInUse = false;

static void Render(Tetris* game)
{
	if (IsInUse) return;
	IsInUse = true;

	printf("\033[H");

	TetrisCell** board = game->board;

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
			printf("      Score: %d", game->Score);

		printf("\n");
	}

	printf("+");
	for (int i = 0; i < COLS * 2; i++)
		printf("-");
	printf("+\n");
	fflush(stdout);

	IsInUse = false;
}

static int thread_worker(void* arg) {
	Tetris* game = (Tetris*)arg;

	clock_t last = clock();
	int interval = INTERVAL;
	while (!game->game_over) {
		if (clock() - last >= interval) {
			last = clock();
			TimerDown(game);
			Render(game);

			interval = INTERVAL - game->Score / SCORE_INFLUENCE_ON_INTERVAL;
			if (interval < MIN_INTERVAL)
				interval = MIN_INTERVAL;
		}
	}

	printf("Game over\n");

	return 0;
}


int main()
{
#ifdef __STDC_NO_THREADS__
	printf("Threads are not supported by this compiler.\n");
	return 1;
#endif

	printf("\033[?25l");
	printf("\033[2J");

	srand(time(NULL));
	thrd_t thread_id;

	Tetris game = new_tetris();
	Render(&game);

	if (thrd_create(&thread_id, thread_worker, &game) != thrd_success) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	while (!game.game_over) {
		switch (_getch()) {
		case ' ':
			FullDown(&game);
			Render(&game);
			break;

		case 72:
			if (Rotate(&game))
				Render(&game);
			break;
		case 80:
			if (Down(&game))
				Render(&game);
			break;
		case 75:
			if (Left(&game))
				Render(&game);
			break;
		case 77:
			if (Right(&game))
				Render(&game);
			break;
		}
	}

	int thread_result;
	thrd_join(thread_id, &thread_result);
	return thread_result;
}