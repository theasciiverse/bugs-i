/***********************************************************/
/* BUGS v1.3  Org by SRN at SUNY CT at Utica/Rome July 10,1991*/
/*          	later mods at UNCW (seq.uncwil.edu)        */
/***********************************************************/
/* I, being of a somewhat sound mind, hearby place this in*/
/* in the great world of public domain 11/15/91*/

#include "bugs.h"
#include "info.h"
#include "loop.h"
#include "screen.h"
#include "startup.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static unsigned int best;       /* best score, kept between games */

/* Every folder on the way to dir, as mkdir -p. */
static int make_dirs(char *dir)
{
    char *p;
    for (p = dir + 1; *p; p++)
        if (*p == '/')
        {
            *p = '\0';
            if (mkdir(dir, 0755) != 0 && errno != EEXIST)
                return 0;
            *p = '/';
        }
    return mkdir(dir, 0755) == 0 || errno == EEXIST;
}

/* $XDG_DATA_HOME/bugs-i/best, or ~/.local/share/bugs-i/best */
static int best_path(char *buf, size_t len, int make)
{
    const char *data = getenv("XDG_DATA_HOME");
    const char *home = getenv("HOME");
    char dir[4096];
    if (data != NULL && data[0] == '/')
        snprintf(dir, sizeof dir, "%s/bugs-i", data);
    else if (home != NULL && home[0] != '\0')
        snprintf(dir, sizeof dir, "%s/.local/share/bugs-i", home);
    else
        return 0;
    if (make && !make_dirs(dir))
        return 0;
    snprintf(buf, len, "%s/best", dir);
    return 1;
}

static void load_best(void)
{
    char path[4200];
    FILE *f;
    if (best_path(path, sizeof path, 0) && (f = fopen(path, "r")) != NULL)
    {
        if (fscanf(f, "%u", &best) != 1)
            best = 0;
        fclose(f);
    }
}

static void save_best(void)
{
    char path[4200];
    FILE *f;
    if (best_path(path, sizeof path, 1) && (f = fopen(path, "w")) != NULL)
    {
        fprintf(f, "%u\n", best);
        fclose(f);
    }
}

static void quit_signal(int e)  /* Ctrl+C, Ctrl+\, SIGTERM, SIGHUP */
{
    (void) e;
    endwin();
    _exit(EXIT_SUCCESS);
}

static void centre(int y, const char *s)
{
    mvprintw(oy + y, ox + (FRAME_W - (int) strlen(s)) / 2, "%s", s);
}

static void title_text(int field, unsigned int level, unsigned int speed)
{
    char line[64];
    clear();
    attron(A_BOLD);
    centre(1, "B U G S   I");
    attroff(A_BOLD);
    centre(2, "Scott Noecker, 1991");
    centre(4, "Clear the board of bugs:");
    centre(5, "line up four of a letter, a bug among them.");
    move(oy + 7, ox + 9);
    addch(glyph('A', 1));
    addch(glyph('L', 1));
    addch(glyph('S', 1));
    printw(" bugs    ");
    addch(glyph('A', 0));
    addch(glyph('L', 0));
    addch(glyph('S', 0));
    printw(" pieces");

    snprintf(line, sizeof line, "%s Level  < %2u >", field == 0 ? ">" : " ", level);
    centre(9, line);
    snprintf(line, sizeof line, "%s Speed  < %2u >", field == 1 ? ">" : " ", speed);
    centre(10, line);
    if (best)
    {
        snprintf(line, sizeof line, "Best score %u", best);
        centre(12, line);
    }
    centre(14, "Arrows or h j l move   Up, x, s turn");
    centre(15, "z, a turn back   Space drops   p pauses");
    centre(17, "Enter to play   q to quit");
    mvprintw(oy + FRAME_H - 1, ox + FRAME_W - 4, "S.");
    refresh();
}

/* The title screen: level and speed. 0 to quit. */
static int title(void)
{
    static unsigned int level = 0, speed = 1;
    int field = 0, ch;
    nodelay(stdscr, FALSE);
    title_text(field, level, speed);
    for (;;)
    {
        ch = getch();
        switch (ch)
        {
        case KEY_RESIZE:
            layout();
            break;
        case KEY_UP:
        case KEY_DOWN:
        case '\t':
            field = !field;
            break;
        case KEY_LEFT:
        case 'j':              /* as in 1991: j down, k up */
            if (field == 0 && level > 0)
                level--;
            else if (field == 1 && speed > 1)
                speed--;
            break;
        case KEY_RIGHT:
        case 'k':
            if (field == 0 && level < 20)
                level++;
            else if (field == 1 && speed < 3)
                speed++;
            break;
        case '\n':
        case '\r':
        case KEY_ENTER:
        case ' ':
            lv = level;
            sp = speed;
            return 1;
        case 'q':
        case 27:
            return 0;
        }
        title_text(field, level, speed);
    }
}

/* A message on the board, in standout, as the 1991 "Hit Return for Next
   Level". Waits for Enter or space (1) or q or Esc (0). */
static int board_message(const char *lines[], int n)
{
    int ch, i;
    nodelay(stdscr, FALSE);
    for (;;)
    {
        wattron(w, A_STANDOUT);
        for (i = 0; i < n; i++)
            mvwprintw(w, 3 + i, (10 - (int) strlen(lines[i])) / 2, "%s", lines[i]);
        wattroff(w, A_STANDOUT);
        board_show();
        ch = key();
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER || ch == ' ')
            break;
        if (ch == 'q' || ch == 27)
            break;
    }
    nodelay(stdscr, TRUE);
    return ch != 'q' && ch != 27;
}

static void play(void)
{
    score = 0;
    piece = 0;
    clear();
    redraw();
    newstart();                 /*set up gameboard */

    for (;;)
    {                           /*forever */
        int end;
        v = lv * 4 + 4;         /*number of bugs formula */
        start();                /*put bugs on board */
        end = loop();           /*gameplay */
        if (end == GAME_OVER)
        {
            char s[16], b[16];
            const char *over[] = { "Game", "Over", "", s, "", b, "", "Enter" };
            snprintf(s, sizeof s, "%u", score);
            if (score > best)
            {
                best = score;
                save_best();
                snprintf(b, sizeof b, "Best!");
            }
            else
                snprintf(b, sizeof b, "Best %u", best);
            board_message(over, 8);
            return;
        }
        if (end == QUIT)
        {
            if (score > best)
            {
                best = score;
                save_best();
            }
            return;
        }
        if (lv != 21)           /*finished level here, maxlv=21 */
            lv++;
        const char *next[] = { "Hit", "Return", "for Next", "Level" };
        stats();
        if (!board_message(next, 4))
            return;
    }
}

int main(void)
{
    signal(SIGQUIT, quit_signal);       /*control-\ */
    signal(SIGINT, quit_signal);        /*control-c */
    signal(SIGTERM, quit_signal);
    signal(SIGHUP, quit_signal);

    srandom((unsigned) time(NULL) ^ (unsigned) getpid());
    /* 1991: srandom(getpid() / 3 + 1), "Yeah, I know, should've used
       time." */
    if (getuid() == 952)        /* Scott's own account had the k key */
        srn = 1;

    load_best();
    initscr();                  /*set up term for curses */
    noecho();
    cbreak();                   /*no return needed for input */
    keypad(stdscr, TRUE);
    set_escdelay(25);           /* Esc answers at once */
    curs_set(0);
    colours();
    layout();

    while (title())
        play();

    endwin();
    return EXIT_SUCCESS;
}

void fatal(char *s)             /*a curses error we cannot play past */
{
    endwin();                   /*clean up */
    printf("%s \n", s);
    exit(EXIT_FAILURE);
}
