#include <curses.h>

#define BLANK ' '

int a[17][9];                   /* Array so side pieces & bugs can
                                   be found */

WINDOW *w;                      /* window is gameboard */

struct pos
{
    int x, y;
};

struct pe
{
    int x, y;
    chtype c;
} p1[2];

struct pe *p2;

short cp;
unsigned int v, score, lv, piece, tcount, sp, sc, srn;
        /* current position state, bugs, score, level, piece count,
           speed, bugs removed */
