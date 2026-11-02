/* Checks that a bug is cleared by pieces of its own letter, in a column
   and in a row. Bugs are drawn in reverse video and pieces are not, so
   the match must compare letters, not what the screen holds with its
   attributes (ncurses' winch keeps them; 1991's BSD curses did not).
   Runs on a dummy terminal. */

#include "../info.h"
#include "../meat.h"
#include <stdio.h>
#include <stdlib.h>

static int failures;

static void board(void)
{
    int x, y;
    werase(w);
    box(w, '|', '"');
    for (y = 1; y < 17; y++)
        for (x = 1; x < 9; x++)
            mvwaddch(w, y, x, ' '), a[y][x] = 0;
}

static void bug(int y, int x, chtype ch)
{
    wstandout(w);
    mvwaddch(w, y, x, ch);
    wstandend(w);
    a[y][x] = 3;
    v++;
}

static void check(const char *what, int ok)
{
    printf("%s: %s\n", ok ? "ok" : "FAIL", what);
    if (!ok)
        failures++;
}

int main(void)
{
    FILE *null = fopen("/dev/null", "w");
    SCREEN *s = newterm("xterm", null, stdin);
    if (s == NULL)
        return 2;
    set_term(s);
    w = newwin(18, 10, 0, 0);

    /* a bug at the bottom of column 3, three pieces of its letter on it */
    board();
    v = 0;
    bug(16, 3, 'L');
    mvwaddch(w, 15, 3, 'L');
    mvwaddch(w, 14, 3, 'L');
    mvwaddch(w, 13, 3, 'L');
    tcount = sc = 0;
    cols(3);
    check("bug cleared by three pieces above it", v == 0);
    check("column emptied", (mvwinch(w, 16, 3) & A_CHARTEXT) == ' ');

    /* a bug in row 16 with three pieces of its letter beside it */
    board();
    v = 0;
    bug(16, 4, 'S');
    mvwaddch(w, 16, 1, 'S');
    mvwaddch(w, 16, 2, 'S');
    mvwaddch(w, 16, 3, 'S');
    tcount = sc = 0;
    row(16);
    check("bug cleared by three pieces beside it", v == 0);

    /* different letters are left alone */
    board();
    v = 0;
    bug(16, 3, 'A');
    mvwaddch(w, 15, 3, 'L');
    mvwaddch(w, 14, 3, 'L');
    mvwaddch(w, 13, 3, 'L');
    tcount = sc = 0;
    cols(3);
    check("bug of another letter stays", v == 1);

    endwin();
    delscreen(s);
    return failures ? 1 : 0;
}
