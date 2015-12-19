/***********************************************************/
/* BUGS v1.3  Org by SRN at SUNY CT at Utica/Rome July 10,1991*/
/*          	later mods at UNCW (seq.uncwil.edu)        */
/***********************************************************/
/* I, being of a somewhat sound mind, hearby place this in*/
/* in the great world of public domain 11/15/91*/

#include "bugs.h"
#include "info.h"
#include "loop.h"
#include "startup.h"
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    signal(SIGQUIT, fatal_err); /*control-/ */
    signal(SIGINT, fatal_err);  /*control-c */


    srandom(getpid() / 3 + 1);  /*if you don't set different seeds,
                                   numbers will always be the same */
    /*Yeah, I know, should've used time.bugs3 does */
    if (getuid() == 952)
        srn = 1;

    initscr();                  /*set up term for curses */
    signal(SIGTSTP, fatal_err); /*control-z */
    noecho();
    crmode();                   /*no return needed for input */
    curs_set(0);

    lv = get_option("Level", 0, 20);
    sp = get_option("Speed", 1, 3);

    clear();
    mvprintw(LINES - 1, COLS - 4, "S.");        /*signature! */
    refresh();                  /*update screen */
    newstart();                 /*set up gameboard */
    bleed(0, 1);                /*unblock stdin */

    while (1)
    {                           /*forever */
        int j;
        v = lv * 4 + 4;         /*number of bugs formula */
        start();                /*put bugs on board */
        loop();                 /*gameplay */
        if (lv != 21)           /*finished level here, maxlv=21 */
            lv++;
        wstandout(w);
        mvwprintw(w, 3, 3, "Hit");
        mvwprintw(w, 4, 2, "Return");
        mvwprintw(w, 5, 1, "for Next");
        mvwprintw(w, 6, 2, "Level");
        wstandend(w);
        wrefresh(w);
        while ((j = wgetch(w)) != '\n');
    }

    return EXIT_SUCCESS;
}

void fatal(char *s)             /*End of Game or user quit if we're here */
{
    move(LINES - 1, 0);
    refresh();
    bleed(0, 0);                /* block stdin, never leave with stdin unblocked! */
    endwin();                   /*clean up */
    printf("%s \n", s);
    exit(0);
}

void fatal_err(int e)           /* handle ctrl-c or z */
{
    (void) e;
    move(LINES - 1, 0);
    refresh();
    bleed(0, 0);
    endwin();
    exit(1);
}

int get_option(char *s, int min, int max)       /*set level & speed */
{
    int ch, i = min;

    mvprintw(0, 0, "%s : %02d", s, i);
    refresh();
    while ((ch = getch()) != '\n')
    {
        switch (ch)
        {
        case 'j':
            if (i > min)
                i--;
            break;

        case 'k':
            if (i < max)
                i++;
            break;
        }
        mvprintw(0, 0, "%s : %02d", s, i);
        refresh();
    }
    return i;
}

void bleed(int filed, int xof)  /*control blocking of stdout (or any fd) */
{
    int flags;
    if ((flags = fcntl(filed, F_GETFL, 0)) < 0)
        fatal("fcntl - get flag");

    if (xof)                    /*if xof is set, don't block filed */
    {
#ifndef LINUX
        if (fcntl(filed, F_SETFL, flags | FNDELAY) < 0)
#else
        if (fcntl(filed, F_SETFL, flags | O_NDELAY) < 0)
#endif /* LINUX */
            fatal("fcntl - set");
    }
    else                        /* block filed */
#ifndef LINUX
    if (fcntl(0, F_SETFL, flags & ~FNDELAY) < 0)        /*block stdin */
#else
    if (fcntl(0, F_SETFL, flags & ~O_NDELAY) < 0)       /*block stdin */
#endif /* LINUX */
        fatal("fcntl - reset");
}
