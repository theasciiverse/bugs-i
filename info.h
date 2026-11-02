#include <curses.h>

#define BLANK ' '

/* The letter in a board cell, without its attributes. Bugs are drawn in
   reverse video and pieces are not; 1991's BSD curses returned only the
   letter from winch, ncurses returns the attributes too, so comparing
   what mvwinch gives made a bug never match its own letter. */
#define letter(y, x) (mvwinch(w, (y), (x)) & A_CHARTEXT)

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
