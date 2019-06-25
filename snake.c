#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <ncurses.h>

const int size = 20;

void swap(char *a, char *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

void swapi(int *a, int *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

struct pos {
	int i;
	int j;
};

struct snake {
	int isAlive; // жива ли змейка
	int length;
	struct pos *c_pos;

	char head;
	char body;
	char tail;
};

// проверка на конец игры

void checkGameOver(char map[][size], struct snake *S) {
	if (map[S->c_pos[0].i][S->c_pos[0].j] == '-' ||
		map[S->c_pos[0].i][S->c_pos[0].j] == '|' ||
		map[S->c_pos[0].i][S->c_pos[0].j] == S->body ||
		map[S->c_pos[0].i][S->c_pos[0].j] == S->tail) S->isAlive = 0;
}

// создание еды

void foodGeneration(char map[][size], struct snake *S) {
	int i = 1 + rand() % (size - 2);
	int j = 1 + rand() % (size - 2);
    
    // генерация размера еды
    
	char food = '1' + rand() % 3;
    
    // проверка на попадание еды на змейку
    
	while (map[i][j] == S->body || map[i][j] == S->head || map[i][j] == S->tail) {
		i = 1 + rand() % (size - 2);
		j = 1 + rand() % (size - 2);
	}

	map[i][j] = food;
}

// прорисовка карты

void drawMap(char map[][size]) {
    clear();

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
            printw("%c ", map[i][j]);
		}

		printw("\n");
	}
}

void init(char map[][size], struct snake *S) {
	S->isAlive = 1;
	S->length = 3; // первоначальная длянна змейки

	S->head = '@';
	S->body = '#';
	S->tail = '*';

	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j) {
			map[i][j] = ' ';

			if (j == size - 1 || j == 0) map[i][j] = '|';
			if (i == size - 1 || i == 0) map[i][j] = '-';
		}

	S->c_pos[0].i = size / 2;
	S->c_pos[0].j = S->c_pos[0].i;

	map[S->c_pos[0].i][S->c_pos[0].j] = S->head;

	for (int k = 1; k < S->length; ++k) {
		S->c_pos[k].i = S->c_pos[k - 1].i + 1;
		S->c_pos[k].j = S->c_pos[0].j;

		map[S->c_pos[k].i][S->c_pos[k].j] = S->body;
	}

	S->c_pos[S->length - 1].i = S->c_pos[S->length - 2].i + 1;
	S->c_pos[S->length - 1].j = S->c_pos[S->length - 2].j;

	map[S->c_pos[S->length - 1].i][S->c_pos[S->length - 1].j] = S->tail;
}

int main(void) {
	srand(time(0));

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);

	int di = -1, dj = 0;
	int i, j;
	int cost = 0;

	struct snake S;
	struct pos cp[(size - 2) * (size - 2)];
	S.c_pos = &cp[0];

	char M[size][size];
	char input = 'W';
	char lInput = input;

	init(M, &S);
	foodGeneration(M, &S);

	while (S.isAlive) {
		drawMap(M);

        input = getch();
        input = toupper(input);

        switch (input) {
            case 'W': if (lInput != 'S') { di = -1; dj = 0; lInput = input; } break;
            case 'A': if (lInput != 'D') { di = 0; dj = -1; lInput = input; } break;
            case 'S': if (lInput != 'W') { di = 1; dj = 0; lInput = input; } break;
            case 'D': if (lInput != 'A') { di = 0; dj = 1; lInput = input; } break;
        }

		i = S.c_pos[0].i;
		j = S.c_pos[0].j;

		S.c_pos[0].i += di;
		S.c_pos[0].j += dj;

		checkGameOver(M, &S);

		if (M[S.c_pos[0].i][S.c_pos[0].j] > '0' && M[S.c_pos[0].i][S.c_pos[0].j] < '4') {
			cost = (int)M[S.c_pos[0].i][S.c_pos[0].j] - 48;

			M[S.c_pos[0].i][S.c_pos[0].j] = ' ';

			foodGeneration(M, &S);
		}

		swap(&M[S.c_pos[0].i][S.c_pos[0].j], &M[i][j]);

		if (cost) {
			for (int k = 1; k < S.length - 1; k++) {
				swap(&M[S.c_pos[k].i][S.c_pos[k].j], &M[i][j]);

				swapi(&S.c_pos[k].i, &i);
				swapi(&S.c_pos[k].j, &j);
			}

			S.length++;

			S.c_pos[S.length - 1].i = S.c_pos[S.length - 2].i;
			S.c_pos[S.length - 1].j = S.c_pos[S.length - 2].j;

			S.c_pos[S.length - 2].i = i;
			S.c_pos[S.length - 2].j = j;

			M[S.c_pos[S.length - 2].i][S.c_pos[S.length - 2].j] = S.body;

			cost--;
		}
		
		else {
			for (int k = 1; k < S.length; k++) {
				swap(&M[S.c_pos[k].i][S.c_pos[k].j], &M[i][j]);

				swapi(&S.c_pos[k].i, &i);
				swapi(&S.c_pos[k].j, &j);
			}
		}

		usleep(350000);
	}

	printw("\nYou loose!\n");

    nodelay(stdscr, FALSE);
    
	getch();
    endwin();

	return 0;
}