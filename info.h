#include <curses.h>

#define BLANK ' '

extern int a[17][9];            /* Array so side pieces & bugs can
                                   be found */

extern WINDOW *w;               /* window is gameboard */

struct pos
{
    int x, y;
};

struct pe
{
    int x, y;
    chtype c;
};

extern struct pe p1[2];
extern struct pe *p2;

extern short cp;
extern unsigned int v, score, lv, piece, tcount, sp, sc, srn;
        /* current position state, bugs, score, level, piece count,
           speed, bugs removed */
