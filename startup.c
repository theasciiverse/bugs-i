#include "startup.h"
#include "info.h"
#include "bugs.h"
#include "screen.h"

chtype c[3];                      /* Game piece array */

void newstart(void)
{
    c[0] = 'A';                 /* Game pieces */
    c[1] = 'L';
    c[2] = 'S';

    p2 = &p1[1];                /*pointers not really necessary here
                                   for that matter neither is array */

    /* A pad, not a window: ncurses shrinks windows that no longer fit when
       the terminal gets smaller, and does not grow them back, so the board
       (whose contents are the game) lost rows after a small resize. Pads
       are left alone; board_show() puts it on the screen. */
    if (w == NULL)
        w = newpad(18, 10);     /*make gameboard */
    if (w == (WINDOW *) NULL)
        fatal("memory error");  /*trouble- no memory! */
    nodelay(stdscr, TRUE);      /* the board never waits for a key */

    box(w, 124, 34);

    mvwprintw(w, 0, 0, "___!  !___");   /*change box a little */
    mvwaddch(w, 17, 0, 34);
    mvwaddch(w, 17, 9, 34);
}

void start(void)                    /*call this every new level */
{
    unsigned int i, x, y, z, ch;
    for (y = 1; y < 17; y++)    /*zap board and a[][] */
        for (x = 1; x < 9; x++)
            mvwaddch(w, y, x, BLANK), a[y][x] = 0;
    board_show();
    for (i = 0; i < v; i++)
    {
        do
        {
            z = rx(3, 0);       /*which char? */
            x = rx(8, 1);
            y = rx(14, 4);
            ch = letter(y, x);
            if (letter(y + 3, x) == c[z]
             || letter(y - 3, x) == c[z]
             || letter(y, x + 3) == c[z]
             || letter(y, x - 3) == c[z])       /* can't have 4 in a row or col to start */
                ch = 0;

        }
        while (ch != BLANK);
        mvwaddch(w, y, x, glyph(c[z], 1));      /*place on board, a bug */
        a[y][x] = 3;            /* show bug in a[][] */
        board_show();
    }
    nextpiece();                /*show next piece */
}

void nextpiece(void)
{
    static chtype na, nb;       /*remember every time */
    p1->c = glyph(c[na], 0);    /*put old chars in play */
    p2->c = glyph(c[nb], 0);
    na = rx(3, 0);              /* make new next piece & display it */
    nb = rx(3, 0);
    show_next(c[na], c[nb]);
}
