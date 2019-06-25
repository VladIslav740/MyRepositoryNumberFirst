#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>

const int N = 10; // размер карты

void swap(char *a, char *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

struct pos {
    int i;
    int j;
};

// прорисовка карты

void draw(char M[][N]) {
    clear();

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            printw("%c ", M[i][j]);

        printw("\n");
    }
}

void init(char M[][N], struct pos *p_pos, int *k) {
    FILE *f; // подключаем карту, файловый дискриптор
    char ch; // вспомогат перемен для считывания карты

    *k = 0; // кол-во ящиков 0

    f = fopen("level.txt", "rb"); // открытие файла для чтения

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            ch = fgetc(f); // считывает символ из файла

            if (ch != '\n') M[i][j] = ch; // если не перевод строки, то наносим на карту
            else j--; // инече останемся на том же столбце

            if (M[i][j] == '&') { // если встретили пресонажа инициализируем позицию
                p_pos->i = i;
                p_pos->j = j;
            }
            else if (M[i][j] == 'x') (*k)++; // иначе если нашли место для ящика увеличим кол-во ящиков
        }

    fclose(f);
}

int main(void) {
    initscr(); // перевод терминала в режим инкерсис
    cbreak();  // обработка нажатий без Enter
    noecho();  // отмена вывода на экран нажатых клавиш

    int di = 0, dj = 0; // дельты движения, на сколько сдвигается персонаж
    int k;              // кол-во ящиков и крестов
    int run = 1;        // игра идет

    struct pos p_pos;   // позиция кладовщика
	
    char M[N][N];       // карта 
    char input;         // ввод направления

    init(M, &p_pos, &k);

    while (run) {
        draw(M);

        input = getch();
        input = toupper(input);

        switch (input) {
            case 'W': di = -1; dj = 0; break;
            case 'A': di = 0; dj = -1; break;
            case 'S': di = 1; dj = 0;  break;
            case 'D': di = 0; dj = 1;  break;
            case 'R':

                init(M, &p_pos, &k);
                di = dj = 0;
                draw(M); break;

            case 'X': run = 0; break;

            default: di = dj = 0; // никуда не двигаемся
        }
		
        p_pos.i += di; // смещение кладовщика
        p_pos.j += dj; // смещение кладовщика
		
		// взаимодействие ящиков с кладовщком
		
        if (M[p_pos.i][p_pos.j] == ' ' || M[p_pos.i][p_pos.j] == '@') {  // либо пустота либо ящик
            if (M[p_pos.i + di][p_pos.j + dj] == ' ') {// перемещение кладовщика с ящиком
                swap(&M[p_pos.i + di][p_pos.j + dj], &M[p_pos.i][p_pos.j]);
                swap(&M[p_pos.i - di][p_pos.j - dj], &M[p_pos.i][p_pos.j]);

                /*if (checkGameOver(M, p_pos.i + di + di, p_pos.j + dj + dj)) {

                    draw(M);

                    printw("\nLOOSEER!\n");

                    run = 0;

                }*/
            }
            else if (M[p_pos.i + di][p_pos.j + dj] == 'x' && M[p_pos.i][p_pos.j] == '@') { // если на "х" то ящик пропадает и "х" меняется на точку
                M[p_pos.i + di][p_pos.j + dj] = '.';
                M[p_pos.i][p_pos.j] = '&';
                M[p_pos.i - di][p_pos.j - dj] = ' ';

                k--;
            }
            else if (M[p_pos.i][p_pos.j] != '@') // перемещение кладовщика без ящика 
                swap(&M[p_pos.i - di][p_pos.j - dj], &M[p_pos.i][p_pos.j]);

            else { // остаемся на месте если ни одно из этих условий
                p_pos.i -= di;
                p_pos.j -= dj;
            }
        }
        else {
            p_pos.i -= di;
            p_pos.j -= dj;
        }

        if (k == 0) { // если ящики на местах то гг
            draw(M);
            printw("\nVICTORY!\n");
			getch();
            run = 0;
        }
    }

    endwin(); // выход из инкерсиса
	
    return 0;
}