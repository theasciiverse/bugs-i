#include "loop.h"
#include "info.h"
#include "bugs.h"
#include "meat.h"
#include "startup.h"
#include "screen.h"
#include <time.h>

struct pos npos[] = {
    { 1,  1},
    {-1, -1}
};                              /*used in turning chips */

void change(int);

/* Plays one level. Returns LEVEL_DONE when every bug is gone, GAME_OVER
   when a new piece has no room, QUIT when the player leaves (q or Esc). */
int loop(void)
{
    int r;
    piece = 0;                  /* haven't played yet */
    stats();                    /*update stats */
    while (v != 0)
    {                           /*till no bugs left */
        cp = 0;
        nextpiece();
        p1->y = p2->y = 0;      /*place at top of board */
        p1->x = 4;
        p2->x = 5;

        mvwaddch(w, p1->y, p1->x, p1->c);
        mvwaddch(w, p2->y, p2->x, p2->c);
        board_show();
        if (letter(1, 5) != BLANK || letter(1, 4) != BLANK)
            /*can't move down, tough luck */
            return GAME_OVER;

        if ((r = loop2()) != 0) /*loop here till piece is place */
            return r;
        meat();                 /*checks for removal and removes pieces if necessary
                                   (garbage man) */
        piece++;                /*used a piece */
        if (sc)
        {                       /*figure out score. if sc=0 we didn't remove any bugs */
            int sco = sp;
            if (sc > 6)
                sc = 6;
            for (; sc > 1; --sc)
                sco = sco * 2;
            score += sco * 100;
            sc = 0;             /*score =  n(sc+1)=n(sc)*200; */
            stats();
        }
    }
    return LEVEL_DONE;
}

/* The pause between steps when no key is waiting; a piece drops one row
   after five of them. "play speed will increase every 10 pieces", says
   the 1992 README, which the code never did: each ten pieces of a level
   now take a tenth off, down to a floor. */
static long step_ns(void)
{
    long base = 250000000L - sp * 50000000L;
    long ns = base * 10 / (10 + piece / 10);
    return ns < 40000000L ? 40000000L : ns;
}

static int pause_game(void)     /* 1 to leave the game */
{
    int ch;
    mvprintw(oy + FRAME_H - 3, ox, "BUGS I");
    mvprintw(oy + FRAME_H - 2, ox, "1.3");
    mvprintw(oy + 9, ox, "Paused: p to go on");
    refresh();
    nodelay(stdscr, FALSE);
    while ((ch = key()) != 'p')
    {
        if (ch == 'q' || ch == 27)
            break;
        if (ch == ERR)          /* a resize redrew the screen */
        {
            mvprintw(oy + FRAME_H - 3, ox, "BUGS I");
            mvprintw(oy + FRAME_H - 2, ox, "1.3");
            mvprintw(oy + 9, ox, "Paused: p to go on");
            refresh();
        }
    }
    nodelay(stdscr, TRUE);
    mvprintw(oy + FRAME_H - 3, ox, "      ");
    mvprintw(oy + FRAME_H - 2, ox, "   ");
    mvprintw(oy + 9, ox, "                  ");
    refresh();
    return ch != 'p';
}

/* Moves the piece in play until it lands. 0 when it has, QUIT when the
   player leaves. Keys: arrows or h, j, l to move; Up, x or s to turn
   clockwise, z or a the other way; space to drop; p to pause; q or Esc
   to leave the game. */
int loop2(void)
{
    int i, dflg = 0;
    short tme = 0;              /*time counter */
    struct timespec t = { .tv_sec = 0, .tv_nsec = 0 };

    while (!dflg)
    {
        i = key();
        if (i == 'q' || i == 27)
            return QUIT;
        switch (i)
        {
        case 'h':
        case KEY_LEFT:
            mvcheck(0, -1);
            break;

        case 'l':
        case KEY_RIGHT:
            mvcheck(0, 1);
            break;

        case 'k':
            if (srn)
                mvcheck(-1, 0);
            break;

        case 'j':
        case KEY_DOWN:
            dflg = mvcheck(1, 0);       /*if dflag is set we hit something */
            tme = 0;            /*reset timer */
            break;

        case 'a':              /*rotate counter-clockwise */
        case 'z':
            change(-1);
            break;

        case 's':              /*rotate clockwise */
        case 'x':
        case KEY_UP:
            change(1);
            break;

        case ' ':
            while (mvcheck(1, 0) != -1)
                board_show();
            dflg = 1;
            break;

        case 'p':
            if (pause_game())
                return QUIT;
            break;
        default:
            t.tv_nsec = step_ns();
            nanosleep(&t, NULL);
            break;
        }
        if (i != ERR)           /*ERR is generated if no input */
            board_show();

        tme++;                  /*increase timer */
        if (tme > 4)            /*No input for awhile? */
            dflg = mvcheck(1, 0), tme = 0, board_show();
    }
    if (dflg && !(cp & 1))
    {                           /*if we're sideways set a[][] */
        a[p1->y][p1->x] = 1;
        a[p2->y][p2->x] = 2;
    }
    return 0;
}

int mvcheck(int y1, int x1)     /*see if we can move or not */
{
    int a = BLANK, b = BLANK;

    /*we don't want to examine the other half of piece */
    if (p1->y + y1 != p2->y || p1->x + x1 != p2->x)
        a = letter(p1->y + y1, p1->x + x1);
    if (p2->y + y1 != p1->y || p2->x + x1 != p1->x)
        b = letter(p2->y + y1, p2->x + x1);
    if (a != BLANK || b != BLANK)
        return -1;              /*no good, if we're going down, we hit
                                   something */

    mvwaddch(w, p1->y, p1->x, BLANK);
    mvwaddch(w, p2->y, p2->x, BLANK);
    p1->y += y1;
    p2->y += y1;
    p1->x += x1;
    p2->x += x1;
    mvwaddch(w, p1->y, p1->x, p1->c);
    mvwaddch(w, p2->y, p2->x, p2->c);
    return 0;                   /*A-ok */
}


void change(int dir)            /*rotate piece */
{
    /*if cp=0 or 2 we are sideways, to test we use !(cp&1)  
       bit-fields were a possibility here, but not worth it */

    int j, l = cp + dir, k = (l & 1);
    int ny, nx, sideflag = 0;
    char temp;

    if (dir == -1)
        dir = 0;

    ny = npos[k].y;             /*next position if turn is successful */
    nx = npos[k].x;

    if (p2->x == 8 && cp & 1)
    {                           /*if we're on the right wall vertically
                                   we need to backoff to turn & check somewhere else */
        j = letter(p1->y, 7);
        sideflag = 1;           /*set a flag */
        nx = 0;
    }
    else
        j = letter(p2->y + ny, p2->x + nx);
    /*not on wall check regularly */

    if (j != BLANK)
        return;                 /*can't turn */

    if (sideflag)
        p1->x -= 1;             /*had to check a first */

    if ((cp + dir) & 1)
    {                           /*swap chars in piece */
        temp = p1->c;
        p1->c = p2->c;
        p2->c = temp;
    }
    mvwaddch(w, p1->y, p1->x, p1->c);
    mvwaddch(w, p2->y, p2->x, BLANK);

    p2->y += ny;                /*add new values to current pos */
    p2->x += nx;
    mvwaddch(w, p2->y, p2->x, p2->c);

    cp = l;
    if (cp < 0)
        cp = 3;
    else if (cp > 3)
        cp = 0;                 /*positions number from 0 to 3 */
}
